#pragma once

#define VIDEO_WIDGET
#include <QLabel>
#include <QRect>
#include <QPixmap>
#include <QResizeEvent>
#include <QWidget>
#include <QOpenGLWidget>

class VideoWidget :public QWidget {
	Q_OBJECT

public:
	explicit VideoWidget(QWidget* parent);
	~VideoWidget() override;
	VideoWidget();
	void SetVideoPixmap(QPixmap& pixmap);

slots
	void slot_GetOneFrame(QImage image);

protected:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent*) override;

private:
	Q_DISABLE_COPY(VideoWidget);
	QPixmap pixmap_;

	double des_w_h_ratio_;
	double src_w_h_ratio_;
	int scale_w_;
	int scale_h_;
};
