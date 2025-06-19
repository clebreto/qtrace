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
    this->tiles.clear();

    qtrTiler tiler;
    tiler.setWholeSize(size);
    int threads = QThread::idealThreadCount();
    tiler.setResolutionX(threads * 2);
    tiler.setResolutionY(threads * 2);

    this->tiles = tiler.tile();
}

void qtrCanvasPrivate::tile(QSizeF size)
{
    this->tile(size.toSize());
}

void qtrCanvasPrivate::render(void)
{
    if(!this->render_method) {
	qWarning() << Q_FUNC_INFO << "No render method set";
	return;
    }

    this->future = QtConcurrent::map(this->tiles, this->render_method);
    this->future_watcher.setFuture(this->future);
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
        qDebug() << "No tiles to render";
        // Draw a warning message
        painter->setPen(Qt::white);
        painter->drawText(rect, Qt::AlignCenter, "No tiles to render\nSize: " + 
                         QString::number(rect.width()) + "x" + QString::number(rect.height()));
    } else {
        int i = 0;
        foreach(qtrTile tile, d->tiles) {
            if (tile.image().isNull()) {
                qDebug() << "Tile" << i << "has null image";
                painter->setPen(Qt::green);
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(tile.tileRect());
            } else {
                painter->drawImage(tile.tileRect(), tile.image());
            }
            i++;
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

    if (d->future_watcher.isRunning()) {
        d->future_watcher.waitForFinished();
    }

    d->tile(this->boundingRect().size().toSize());

    d->render();

    emit minProgressValueChanged();
    emit maxProgressValueChanged();
}

void qtrCanvas::onTileRendered(int)
{
    this->update();

    emit curProgressValueChanged();
}

void qtrCanvas::geometryChanged(const QRectF& current, const QRectF& previous)
{

    if (d->future_watcher.isRunning()) {
        qDebug() << "Cancelling current rendering";
        d->future_watcher.cancel();
    }

    if (d->resize_timer.isActive()) {
        qDebug() << "Stopping existing resize timer";
        d->resize_timer.stop();
    }

    d->resize_timer.start(500);
}

#include "qtrCanvas.moc"

//
// qtrCanvas.cpp ends here
