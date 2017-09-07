#ifndef GLUTTONOUSSNAKE_H
#define GLUTTONOUSSNAKE_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>

class CGluttonousSnake : public QWidget
{
	Q_OBJECT

public:
	enum Direction
	{
		Up        = -1,
		Down	= 1,
		Left      = -2,
		Right	= 2,
	};

public:
	CGluttonousSnake(QWidget *parent = 0);
	~CGluttonousSnake();

public:
	void reset(bool autoStart = true);

	void pause();
	void start();

protected:
	void paintEvent(QPaintEvent *event);
	void timerEvent(QTimerEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:
	void onFailed();

	QList<QPoint>	m_snake;
	QList<QPoint> m_food;
	QList<QPoint> m_barrier;									//障碍
	QList<QPoint> m_armourPiercingBullet;				//穿甲弹

	int						m_nArmourPiercingBullet;		//穿甲弹数量(穿墙)
	int						m_nTimerID;
	int						m_nCreateFoodTimerID;		//生成食物计时器
	int						m_nCreateBarrierTimerID;		//生成障碍计时器
	int						m_nCreateAPBTimerID;		//生成穿甲弹计时器
	Direction				m_eCurrentDirection;		
	Direction				m_eNextDirection;
	int						m_nGridSize;

	bool					m_bIsPause;
};

#endif // GLUTTONOUSSNAKE_H
