#include <QPainter>
#include <QApplication>
#include "match.h"

Match::Match(QWidget *parent)
    : QWidget(parent) {

  x = 0;
  count = 3;
  gameOver = false;
  gameWon = false;
  paused = false;
  gameStarted = false;
  ball = new Ball();
  paddle = new Paddle();
  int ras = 0;
  for(int i = 0; i < count;i++){
  pucksupply[i] = new PuckSupply(230+ras, 10);//позиции сердечек
  ras = ras + 22;
  }
  int k = 0;

  for (int i=0; i<5; i++) {//присвоение позиции для кирпичиков
    for (int j=0; j<6; j++) {
      bricks[k] = new Brick(j*40+30, i*10+50);
      k++;
    }
  }
}

Match::~Match() {

 delete ball;
 delete paddle;

 for (int i=0; i<N_OF_BRICKS; i++) {
   delete bricks[i];
 }
}

void Match::paintEvent(QPaintEvent *e) {//при условиях прописываются game lost victory и тд

  Q_UNUSED(e);

  QPainter painter(this);

  if (gameOver) {

    finishGame(&painter, "Game lost");

  } else if(gameWon) {

    finishGame(&painter, "Victory");
  }
  else if(paused){
    finishGame(&painter, "Press SPACE to resume");
  }
  else{
    drawObjects(&painter);
  }
}

void Match::finishGame(QPainter *painter, QString message) {//отрисовывает событие paintEventer

  QFont font("Courier", 15, QFont::DemiBold);
  QFontMetrics fm(font);
  int textWidth = fm.width(message);

  painter->setFont(font);
  int h = height();
  int w = width();

  painter->translate(QPoint(w/2, h/2));
  painter->drawText(-textWidth/2, 0, message);
}

void Match::drawObjects(QPainter *painter) {//отрисовыка всех объектов

  painter->drawImage(ball->getRect(), ball->getImage());
  painter->drawImage(paddle->getRect(), paddle->getImage());
  for(int i =0; i < count; i++){
  painter->drawImage(pucksupply[i]->getRect(), pucksupply[i]->getImage());
}

  for (int i=0; i<N_OF_BRICKS; i++) {
    if (!bricks[i]->isDestroyed()) {
      painter->drawImage(bricks[i]->getRect(), bricks[i]->getImage());
    }
  }
}

void Match::timerEvent(QTimerEvent *e) {//обнавляет события

  Q_UNUSED(e);

  moveObjects();
  checkCollision();
  repaint();
}

void Match::moveObjects() {

  ball->autoMove();
  paddle->move();
}

void Match::keyReleaseEvent(QKeyEvent *e) {

    int dx = 0;

    switch (e->key()) {
        case Qt::Key_Left:
            dx = 0;
            paddle->setDx(dx);
            break;

        case Qt::Key_Right:
            dx = 0;
            paddle->setDx(dx);
            break;
    }
}

void Match::keyPressEvent(QKeyEvent *e) {

    int dx = 0;

    switch (e->key()) {
    case Qt::Key_Left:

        dx = -1;
        paddle->setDx(dx);

        break;

    case Qt::Key_Right:

        dx = 1;
        paddle->setDx(dx);
        break;

    case Qt::Key_P:

        pauseGame();
        break;

    case Qt::Key_Space:

        startGame();
        unpauseGame();
        break;

    case Qt::Key_Escape:

        qApp->exit();
        break;

    default:
        QWidget::keyPressEvent(e);
    }
}

void Match::startGame() {

  if (!gameStarted) {
    ball->resetState();
    paddle->resetState();

    for (int i=0; i<N_OF_BRICKS; i++) {
      bricks[i]->setDestroyed(false);
    }

    gameOver = false;
    gameWon = false;
    gameStarted = true;
    timerId = startTimer(DELAY);
  }
}
void Match::unpauseGame() {
    if (paused) {

      timerId = startTimer(DELAY);
      paused = false;
    }
}
void Match::pauseGame() {

  if (paused) {

    timerId = startTimer(DELAY);
    paused = false;
  } else {

    paused = true;
    killTimer(timerId);
  }
}

void Match::stopGame() {

  killTimer(timerId);
  gameOver = true;
  gameStarted = false;
}

void Match::victory() {

  killTimer(timerId);
  gameWon = true;
  gameStarted = false;
}


void Match::checkCollision() {

  if (ball->getRect().bottom() > BOTTOM_EDGE) {
    if(count <= 1){
    stopGame();
    }
    else{
        ball->resetState();
        paddle->resetState();
        count = count - 1;
        repaint();
        startGame();
        pauseGame();
  }
}
  for (int i=0, j=0; i<N_OF_BRICKS; i++) {

    if (bricks[i]->isDestroyed()) {
      j++;
    }

    if (j == N_OF_BRICKS) {
      victory();
    }
  }



  if ((ball->getRect()).intersects(paddle->getRect())) {//касание мяча платформы

    int paddleLPos = paddle->getRect().left();
    int ballLPos = ball->getRect().left();

    int first = paddleLPos + 8;
    int second = paddleLPos + 16;
    int third = paddleLPos + 24;
    int fourth = paddleLPos + 32;

    if (ballLPos < first) {
      ball->setXDir(-1);
      ball->setYDir(-1);
    }

    if (ballLPos >= first && ballLPos < second) {
      ball->setXDir(-1);
      ball->setYDir(-1*ball->getYDir());
    }

    if (ballLPos >= second && ballLPos < third) {
       ball->setXDir(0);
       ball->setYDir(-1);
    }

    if (ballLPos >= third && ballLPos < fourth) {
       ball->setXDir(1);
       ball->setYDir(-1*ball->getYDir());
    }

    if (ballLPos > fourth) {
      ball->setXDir(1);
      ball->setYDir(-1);
    }
  }//

  for (int i=0; i<N_OF_BRICKS; i++) {

    if ((ball->getRect()).intersects(bricks[i]->getRect())) {

      int ballLeft = ball->getRect().left();
      int ballHeight = ball->getRect().height();
      int ballWidth = ball->getRect().width();
      int ballTop = ball->getRect().top();

      QPoint pointRight(ballLeft + ballWidth + 1, ballTop);
      QPoint pointLeft(ballLeft - 1, ballTop);
      QPoint pointTop(ballLeft, ballTop -1);
      QPoint pointBottom(ballLeft, ballTop + ballHeight + 1);

      if (!bricks[i]->isDestroyed()) {
        if(bricks[i]->getRect().contains(pointRight)) {
           ball->setXDir(-1);
        }

        else if(bricks[i]->getRect().contains(pointLeft)) {
           ball->setXDir(1);
        }

        if(bricks[i]->getRect().contains(pointTop)) {
           ball->setYDir(1);
        }

        else if(bricks[i]->getRect().contains(pointBottom)) {
           ball->setYDir(-1);
        }
        bricks[i]->setDestroyed(true);
      }
    }
  }
}
