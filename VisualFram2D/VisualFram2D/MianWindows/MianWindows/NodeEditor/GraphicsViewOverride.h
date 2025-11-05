#pragma once
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QtWidgets/QGraphicsView>

namespace QtNodes {

    class BasicGraphicsScene;

    /**
     * @brief A central view able to render objects from `BasicGraphicsScene`.
     */
    class GraphicsViewOverride : public QGraphicsView
    {
        Q_OBJECT
    public:
        struct ScaleRange
        {
            double minimum = 0;
            double maximum = 0;
        };

    public:
        GraphicsViewOverride(QWidget* parent = Q_NULLPTR);
        GraphicsViewOverride(BasicGraphicsScene* scene, QWidget* parent = Q_NULLPTR);

        GraphicsViewOverride(const GraphicsViewOverride&) = delete;
        GraphicsViewOverride operator=(const GraphicsViewOverride&) = delete;

        QAction* clearSelectionAction() const;

        QAction* deleteSelectionAction() const;

        void setScene(BasicGraphicsScene* scene);

        void centerScene();

        /// @brief max=0/min=0 indicates infinite zoom in/out
        void setScaleRange(double minimum = 0, double maximum = 0);

        void setScaleRange(ScaleRange range);

        double getScale() const;

    public Q_SLOTS:
        void scaleUp();

        void scaleDown();

        void setupScale(double scale);

        void onDeleteSelectedObjects();

        void onDuplicateSelectedObjects();

        void onCopySelectedObjects();

        void onPasteObjects();

    Q_SIGNALS:
        void scaleChanged(double scale);

    protected:
        void contextMenuEvent(QContextMenuEvent* event) override;

        void wheelEvent(QWheelEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

        void mousePressEvent(QMouseEvent* event) override;

        void mouseMoveEvent(QMouseEvent* event) override;

        void drawBackground(QPainter* painter, const QRectF& r) override;

        void showEvent(QShowEvent* event) override;

        /// <summary>
        /// 拖拽接收事件
        /// </summary>
        /// <param name="event"></param>
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;

    protected:
        BasicGraphicsScene* nodeScene();

        /// Computes scene position for pasting the copied/duplicated node groups.
        QPointF scenePastePosition();

    private:
        QAction* _clearSelectionAction = nullptr;
        QAction* _deleteSelectionAction = nullptr;
        QAction* _duplicateSelectionAction = nullptr;
        QAction* _copySelectionAction = nullptr;
        QAction* _pasteAction = nullptr;

        QPointF _clickPos;
        ScaleRange _scaleRange;
    };
} // namespace QtNodes
