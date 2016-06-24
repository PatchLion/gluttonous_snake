#include "GluttonousSnake.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTime>
#include <QDebug>
#define DEFAULT_GRID_SIZE			25
#define DEFAULT_INTER					200
#define DEFAULT_FOOD_INTER					2000
#define DEFAULT_BARRIER_INTER				8000
#define DEFAULT_APB_INTER						15000
CGluttonousSnake::CGluttonousSnake(QWidget *parent)
	: QWidget(parent)
	, m_eCurrentDirection(Up)
	, m_eNextDirection(Up)
	//, m_nUpdateInter(DEFAULT_INTER)
	, m_nGridSize(DEFAULT_GRID_SIZE)
	, m_bIsPause(false)
	, m_nArmourPiercingBullet(0)
{
	ui.setupUi(this);

	setFixedSize(500, 500);

	reset();
}

CGluttonousSnake::~CGluttonousSnake()
{

}

void CGluttonousSnake::reset(bool autoStart)
{
	m_bIsPause = false;
	pause();
	m_eCurrentDirection = Up;
	//m_nUpdateInter = DEFAULT_INTER;
	m_nArmourPiercingBullet = 0;

	m_food.clear();
	m_snake.clear();
	m_barrier.clear();
	m_armourPiercingBullet.clear();
	m_snake << QPoint(m_nGridSize / 2, m_nGridSize / 2);

	if (autoStart)
	{
		start();
	}

	update();
}
void CGluttonousSnake::pause()
{
	m_bIsPause = true;
	killTimer(m_nTimerID);
	killTimer(m_nCreateFoodTimerID);
	killTimer(m_nCreateBarrierTimerID);
	killTimer(m_nCreateAPBTimerID);
}

void CGluttonousSnake::start()
{
	m_bIsPause = false;
	m_nTimerID = startTimer(DEFAULT_INTER);
	m_nCreateFoodTimerID = startTimer(DEFAULT_FOOD_INTER);
	m_nCreateBarrierTimerID = startTimer(DEFAULT_BARRIER_INTER);
	m_nCreateAPBTimerID = startTimer(DEFAULT_APB_INTER);
}

void CGluttonousSnake::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);


	//绘制网格
	if (m_nGridSize >= 2)
	{
		const int widthPerUnit = width() / m_nGridSize;
		const int heightPerUnit = height() / m_nGridSize;

		for (int n = 0; n < m_nGridSize; n++)
		{
			painter.setPen(Qt::gray);
			painter.drawLine(0, n*heightPerUnit, width(), n*heightPerUnit);
			painter.drawLine(n*widthPerUnit, 0, n*widthPerUnit, height());
		}

		//绘制蛇
		for (int n = 0; n < m_snake.size(); n++)
		{
			QRect snake(m_snake[n].x() * widthPerUnit, m_snake[n].y() * heightPerUnit, widthPerUnit, heightPerUnit);
		
			if (0 == n)
			{
				//区别头部颜色
				painter.setBrush(QColor(Qt::red).darker(200));
			}
			else
			{
				painter.setBrush(Qt::red);
			}
			
			painter.drawRect(snake);
		}

		//绘制食物
		for (int n = 0; n < m_food.size(); n++)
		{
			QRect food(m_food[n].x() * widthPerUnit, m_food[n].y() * heightPerUnit, widthPerUnit, heightPerUnit);
			painter.setBrush(Qt::blue);
			painter.drawRect(food);
		}
		//绘制障碍
		for (int n = 0; n < m_barrier.size(); n++)
		{
			QRect barrier(m_barrier[n].x() * widthPerUnit, m_barrier[n].y() * heightPerUnit, widthPerUnit, heightPerUnit);
			painter.setBrush(Qt::black);
			painter.drawRect(barrier);
		}
		//绘制障碍
		for (int n = 0; n < m_armourPiercingBullet.size(); n++)
		{
			QRect armourPiercingBullet(m_armourPiercingBullet[n].x() * widthPerUnit, m_armourPiercingBullet[n].y() * heightPerUnit, widthPerUnit, heightPerUnit);
			painter.setBrush(Qt::yellow);
			painter.drawEllipse(armourPiercingBullet);
		}
	}

	painter.setPen(Qt::black);
	painter.drawText(20, 20, QString("Length: %1").arg(m_snake.size()));
	painter.drawText(20, 33, QString("APBullet: %1").arg(m_nArmourPiercingBullet));
}



void CGluttonousSnake::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_nTimerID)
	{
		QPoint headPoint = m_snake[0];

		if (m_eCurrentDirection + m_eNextDirection != 0)
		{
			m_eCurrentDirection = m_eNextDirection;
		}

		switch (m_eCurrentDirection)
		{
		case CGluttonousSnake::Up:
		{
			headPoint.setY(headPoint.y() - 1);
		}
		break;
		case CGluttonousSnake::Down:
		{
			headPoint.setY(headPoint.y() + 1);
		}
		break;
		case CGluttonousSnake::Left:
		{
			headPoint.setX(headPoint.x() - 1);
		}
		break;
		case CGluttonousSnake::Right:
		{
			headPoint.setX(headPoint.x() + 1);
		}
		break;
		}

		if (headPoint.x() < 0 || headPoint.x() >= m_nGridSize
			|| headPoint.y() < 0 || headPoint.y() >= m_nGridSize
			|| m_snake.contains(headPoint)
			|| m_barrier.contains(headPoint))
		{
			bool con = true;
			if (m_barrier.contains(headPoint) && m_nArmourPiercingBullet > 0)
			{
				//撞墙 但是有穿甲弹
				m_nArmourPiercingBullet--;
				m_barrier.removeOne(headPoint);
				con = false;
				m_snake.push_front(headPoint);
				m_snake.removeAt(m_snake.size() - 1);
				
			}

			if (con)
			{
				//检测碰撞
				onFailed();
			}

		}
		else
		{
			
			if (m_food.contains(headPoint))
			{
				//有食物
				m_snake.push_front(headPoint);
				m_food.removeOne(headPoint);
			}
			else
			{
				//没有食物
				m_snake.removeAt(m_snake.size() - 1);
				m_snake.push_front(headPoint);
			}
		
			if (m_armourPiercingBullet.contains(headPoint))
			{
				//吃到穿甲弹
				m_nArmourPiercingBullet++;
				m_armourPiercingBullet.removeOne(headPoint);
			}
			update();
		}
	}
	else if (event->timerId() == m_nCreateFoodTimerID
		|| event->timerId() == m_nCreateBarrierTimerID
		|| event->timerId() == m_nCreateAPBTimerID)
	{
		//定时创建食物或障碍

		//选取随机空白格
		qsrand(QTime::currentTime().msec());
		QList<QPoint> listEmptyPoint;
		for (int x = 0; x < m_nGridSize; x++)
		{
			for (int y = 0; y < m_nGridSize; y++)
			{
				QPoint tempPoint(x, y);
				if (!m_snake.contains(tempPoint)
					&& !m_food.contains(tempPoint)
					&& !m_armourPiercingBullet.contains(tempPoint)
					&& !m_barrier.contains(tempPoint))
				{
					listEmptyPoint << tempPoint;
				}
			}
		}

		//添加食物或障碍
		if (event->timerId() == m_nCreateFoodTimerID)
		{ 
			if (listEmptyPoint.size() == 1)
			{
				m_food << listEmptyPoint[0];
			}
			else
			{
				m_food << listEmptyPoint[qrand() % listEmptyPoint.size()];
			}
		}
		else if (event->timerId() == m_nCreateBarrierTimerID)
		{
			if (listEmptyPoint.size() == 1)
			{
				m_barrier << listEmptyPoint[0];
			}
			else
			{
				m_barrier << listEmptyPoint[qrand() % listEmptyPoint.size()];
			}
		}
		else if (event->timerId() == m_nCreateAPBTimerID)
		{
			if (listEmptyPoint.size() == 1)
			{
				m_armourPiercingBullet << listEmptyPoint[0];
			}
			else
			{
				m_armourPiercingBullet << listEmptyPoint[qrand() % listEmptyPoint.size()];
			}
		}
	}

}

void CGluttonousSnake::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Up)
	{
		m_eNextDirection = Up;
	}
	else if(event->key() == Qt::Key_Down)
	{
		m_eNextDirection = Down;
	}
	else if (event->key() == Qt::Key_Left)
	{
		m_eNextDirection = Left;
	}
	else if (event->key() == Qt::Key_Right)
	{
		m_eNextDirection = Right;
	}
	else if (event->key() == Qt::Key_R)
	{
		reset();
	}
	else if (event->key() == Qt::Key_P)
	{
		if (!m_bIsPause)
		{
			pause();
		}
		else
		{
			start();
		}
	
	}

	update();
	QWidget::keyPressEvent(event);
}

void CGluttonousSnake::onFailed()
{
	const int score = m_snake.size();
	
	QMessageBox::warning(this, "Failed", QString("You lose! Score: %1").arg(score));
	reset(false);
}

