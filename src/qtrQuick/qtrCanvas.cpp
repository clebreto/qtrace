// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#include "qtrCanvas.h"

#include <QtConcurrent>
#include <QtCore>

// ///////////////////////////////////////////////////////////////////
// qtrCanvasPrivate
// ///////////////////////////////////////////////////////////////////

class qtrCanvasPrivate : public QObject
{
    Q_OBJECT

public slots:
    void update(void);

public:
    void tile(QSize size);
    void tile(QSizeF size);

public:
    void render(void);

public:
    qtrTileList tiles;

public:
    qtrRenderer::qtrRenderMethod render_method;

public:
    QFuture<void>        future;
    QFutureWatcher<void> future_watcher;

public:
    QTimer resize_timer;
    QTimer thread_timer;

public:
    int thread_cur;
    int thread_max;

 public:
    qtrCanvas *q;
};

void qtrCanvasPrivate::update(void)
{
    q->setCurNumberOfThreads(QThreadPool::globalInstance()->activeThreadCount());

    this->thread_timer.singleShot(100, this, SLOT(update()));
}

void qtrCanvasPrivate::tile(QSize size)
{
    qDebug() << "=== TILE GENERATION START ===";
    qDebug() << "Requested tile size:" << size;
    
    this->tiles.clear();

    if (size.isEmpty()) {
        qWarning() << "Cannot generate tiles for empty size";
        return;
    }

    qtrTiler tiler;
    tiler.setWholeSize(size);
    int threads = QThread::idealThreadCount();
    qDebug() << "Thread count:" << threads;
    
    int resolution = threads * 2;
    tiler.setResolutionX(resolution);
    tiler.setResolutionY(resolution);
    qDebug() << "Tile resolution:" << resolution << "x" << resolution;

    qDebug() << "Generating tiles...";
    this->tiles = tiler.tile();
    
    qDebug() << "=== TILE GENERATION COMPLETE ===";
    qDebug() << "Total tiles generated:" << this->tiles.size();
    
    if (this->tiles.isEmpty()) {
        qWarning() << "WARNING: No tiles were generated!";
    } else {
        qDebug() << "First tile details:";
        qDebug() << "  - Position:" << this->tiles.first().tileRect().topLeft();
        qDebug() << "  - Size:" << this->tiles.first().tileRect().size();
        qDebug() << "  - Is null?" << this->tiles.first().image().isNull();
    }
}

void qtrCanvasPrivate::tile(QSizeF size)
{
    this->tile(size.toSize());
}

void qtrCanvasPrivate::render(void)
{
    qDebug() << "=== RENDER START ===";
    
    if (!this->render_method) {
        qCritical() << "ERROR: No render method set!";
        return;
    }

    if (this->tiles.isEmpty()) {
        qCritical() << "ERROR: No tiles to render! Tile count is zero.";
        return;
    }

    qDebug() << "Preparing to render" << this->tiles.size() << "tiles";
    qDebug() << "Using render method at address:" << (void*)this->render_method;
    
    try {
        qDebug() << "Starting QtConcurrent::map with" << this->tiles.size() << "tiles";
        this->future = QtConcurrent::map(this->tiles, this->render_method);
        qDebug() << "Future created, setting up watcher...";
        
        this->future_watcher.setFuture(this->future);
        qDebug() << "=== RENDER STARTED SUCCESSFULLY ===";
        qDebug() << "Future progress range:" << this->future_watcher.progressMinimum() 
                 << "to" << this->future_watcher.progressMaximum();
                 
    } catch (const std::exception &e) {
        qCritical() << "EXCEPTION in render:" << e.what();
    } catch (...) {
        qCritical() << "UNKNOWN EXCEPTION in render";
    }
    
    qDebug() << "=== RENDER INITIATION COMPLETE ===";
}

// ///////////////////////////////////////////////////////////////////
// qtrCanvas
// ///////////////////////////////////////////////////////////////////

qtrCanvas::qtrCanvas(QQuickItem *parent) : QQuickPaintedItem(parent), d(new qtrCanvasPrivate)
{
    d->q = this;
    d->render_method = qtrRenderer::newton;

    d->thread_cur = QThreadPool::globalInstance()->activeThreadCount();
    d->thread_max = QThreadPool::globalInstance()->maxThreadCount();

    d->resize_timer.setSingleShot(true);

    connect(&d->future_watcher, SIGNAL(progressValueChanged(int)), this, SLOT(onTileRendered(int)));
    connect(&d->resize_timer, SIGNAL(timeout()), this, SLOT(onResize()));
    
    // Initial resize if we have valid dimensions
    if (width() > 0 && height() > 0) {
        onResize();
    }
}

qtrCanvas::~qtrCanvas(void)
{
    if (d->future_watcher.isRunning()) {
	d->future_watcher.cancel();
	d->future_watcher.waitForFinished();
    }

    delete d;

    d = NULL;
}

void qtrCanvas::setRenderMethod(qtrRenderer::qtrRenderMethod method)
{
    d->render_method = method;
}

void qtrCanvas::setNewtonOrder(int order)
{
    if (d->future_watcher.isRunning())
	d->future_watcher.waitForFinished();

    qtrRenderer::newtonOrder = order;

    d->tile(this->boundingRect().size());
    d->render();
}

int qtrCanvas::newtonOrder(void)
{
    return qtrRenderer::newtonOrder;
}

void qtrCanvas::paint(QPainter *painter)
{
    QRectF rect = boundingRect();

    // Draw background
    painter->fillRect(rect, Qt::red);
    painter->setRenderHints(QPainter::Antialiasing);
    
    // Draw border
    painter->setPen(QPen(Qt::blue, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect.adjusted(1, 1, -1, -1));
    
    // Draw center crosshair
    painter->drawLine(rect.center().x(), 0, rect.center().x(), rect.height());
    painter->drawLine(0, rect.center().y(), rect.width(), rect.center().y());
    
    // Draw tiles if we have any
    if (d->tiles.isEmpty()) {
        QString status = "No tiles to render\n" +
                       QString("Size: %1x%2\n").arg(rect.width()).arg(rect.height()) +
                       "Please wait for rendering to start...";
        
        QFont font = painter->font();
        font.setPointSize(10);
        painter->setFont(font);
        painter->setPen(Qt::white);
        painter->drawText(rect, Qt::AlignCenter, status);
        
        // Draw a border to show the canvas area
        painter->setPen(QPen(Qt::gray, 1, Qt::DotLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect.adjusted(1, 1, -1, -1));
    } else {
        int renderedTiles = 0;
        int i = 0;
        foreach(qtrTile tile, d->tiles) {
            if (tile.image().isNull()) {
                // Draw tile boundary for debugging
                painter->setPen(QPen(Qt::green, 1, Qt::DotLine));
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(tile.tileRect());
            } else {
                painter->drawImage(tile.tileRect(), tile.image());
                renderedTiles++;
            }
            i++;
        }
        
        // Show rendering progress
        if (renderedTiles < d->tiles.size()) {
            QString progress = QString("Rendering: %1/%2 tiles").arg(renderedTiles).arg(d->tiles.size());
            QFont font = painter->font();
            font.setPointSize(8);
            painter->setFont(font);
            painter->setPen(Qt::white);
            painter->fillRect(10, 10, 150, 30, QColor(0, 0, 0, 180));
            painter->drawText(QRect(10, 10, 150, 30), Qt::AlignCenter, progress);
        }
    }
    
    // Force an update to keep the animation going
    update();
}

int qtrCanvas::curNumberOfThreads(void)
{
    return d->thread_cur;
}

int qtrCanvas::maxNumberOfThreads(void)
{
    return d->thread_max;
}

void qtrCanvas::setCurNumberOfThreads(int count)
{
    if(count == d->thread_cur)
	return;

    d->thread_cur = count;

    emit curNumberOfThreadsChanged();
}

void qtrCanvas::setMaxNumberOfThreads(int count)
{
    if(count == d->thread_max)
	return;

    d->thread_max = count;

    emit maxNumberOfThreadsChanged();
}

int qtrCanvas::minProgressValue(void)
{
    return d->future_watcher.progressMinimum();
}

int qtrCanvas::maxProgressValue(void)
{
    return d->future_watcher.progressMaximum();
}

int qtrCanvas::curProgressValue(void)
{
    return d->future_watcher.progressValue();
}

void qtrCanvas::onResize(void)
{
    qDebug() << "\n=== ON RESIZE START ===";
    
    QSize size = this->boundingRect().size().toSize();
    qDebug() << "New size to render:" << size;

    if (size.isEmpty()) {
        qWarning() << "WARNING: Cannot resize to empty size";
        return;
    }

    // Check if we're already rendering
    if (d->future_watcher.isRunning()) {
        qDebug() << "Previous rendering in progress, waiting for completion...";
        // In Qt6, waitForFinished() doesn't take a timeout parameter
        // Instead, we'll use a QEventLoop with a timer
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&d->future_watcher, &QFutureWatcherBase::finished, &loop, &QEventLoop::quit);
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        
        timer.start(1000); // 1 second timeout
        loop.exec();
        
        if (timer.isActive()) {
            // Finished before timeout
            timer.stop();
            qDebug() << "Previous rendering completed";
        } else {
            // Timeout occurred
            qWarning() << "WARNING: Previous rendering did not complete in time!";
            d->future_watcher.cancel();
        }
    }

    // Generate new tiles
    qDebug() << "Generating new tiles...";
    d->tile(size);
    
    // Verify tiles were generated
    if (d->tiles.isEmpty()) {
        qCritical() << "CRITICAL: No tiles generated after resize!";
        return;
    }
    
    qDebug() << "Starting render process...";
    d->render();
    
    // Emit signals for UI updates
    emit minProgressValueChanged();
    emit maxProgressValueChanged();
    
    // Force immediate repaint
    qDebug() << "Forcing UI update...";
    update();
    
    qDebug() << "=== ON RESIZE COMPLETE ===\n";
}

void qtrCanvas::onTileRendered(int progressValue)
{
    qDebug() << "=== TILE RENDERED ===";
    qDebug() << "Progress value:" << progressValue;
    qDebug() << "Progress range:" << d->future_watcher.progressMinimum() 
             << "to" << d->future_watcher.progressMaximum();
    
    // Only update every 10% progress to reduce log spam
    static int lastProgress = -1;
    int currentProgress = (progressValue * 100) / (d->future_watcher.progressMaximum() - d->future_watcher.progressMinimum());
    
    if (currentProgress != lastProgress && currentProgress % 10 == 0) {
        qDebug() << "Rendering progress:" << currentProgress << "%";
        lastProgress = currentProgress;
    }
    
    // Force a repaint
    this->update();
    
    // Notify any connected components about the progress change
    emit curProgressValueChanged();
    
    qDebug() << "=== TILE RENDERED COMPLETE ===";
}

void qtrCanvas::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

    if (newGeometry.size() != oldGeometry.size()) {
        if (d->resize_timer.isActive()) {
            d->resize_timer.stop();
        }

        // Only trigger resize if we have valid dimensions
        if (newGeometry.width() > 0 && newGeometry.height() > 0) {
            d->resize_timer.start(100);  // Reduced delay for better responsiveness
        }
    }
}

#include "qtrCanvas.moc"

//
// qtrCanvas.cpp ends here
