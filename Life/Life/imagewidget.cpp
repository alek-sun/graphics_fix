#include "imagewidget.h"

using std::stack;
using std::pair;
using std::cin;
using std::cout;
using std::endl;

ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent)
{
    recentlyBirthColor.setRgb(160, 100, 230, 250);
    recentlyDieColor.setRgb(160, 100, 230, 150);
    longBirthColor.setRgb(110, 100, 230, 250);
    longDieColor.setRgb(190, 0, 200, 50);
    borderColor.setRgb(0, 0, 0, 255);
    backgroundColor.setRgb(255,255,255,255);
    image = new QImage(width(), height(), QImage::Format_ARGB32);
    bits = image->bits();
}

void ImageWidget::drawHexagonLines(Cell* h)
{
    int i;
    if (h->vert.size() != 6){
        cout << "ERR" << endl;
        return;
    }

    for (i = 0; i < 6; ++i){

        if (i != 5){
            drawLine(h->x0 + h->vert[i].first,
                     h->y0 + h->vert[i].second,
                     h->x0 + h->vert[i+1].first,
                     h->y0 + h->vert[i+1].second,
                     borderColor);
        } else {
            drawLine(h->x0 + h->vert[i].first,
                     h->y0 + h->vert[i].second,
                     h->x0 + h->vert[0].first,
                     h->y0 + h->vert[0].second,
                     borderColor);
        }
    }

}

GameLogic *ImageWidget::getGameLogic() const
{
    return gameLogic;
}

void ImageWidget::setGameLogic(GameLogic *value)
{
    gameLogic = value;
    createHexagonField(gameLogic->m, gameLogic->n);
    gameLogic->changeColors();
}

void ImageWidget::drawText(QString text, int x, int y, int height, QColor color)
{
    QPainter painter;
    painter.begin(image);
    painter.setPen(color);
    painter.setFont(QFont("Helvetica", height, QFont::Bold));
    painter.drawText(x-text.size()*height+2, y-height, text.size()*2*height, 2*height, Qt::AlignCenter | Qt::AlignHCenter, text);
}

void ImageWidget::fillCell(Cell* cell)
{
    switch (cell->getState()){
        case Cell::RECENTLY_BIRTH :
            fillArea(cell->x0, cell->y0, cell->lastColor, recentlyBirthColor, 0);
            cell->lastColor = recentlyBirthColor;
            break;
        case Cell::LONG_BIRTH :
            fillArea(cell->x0, cell->y0, cell->lastColor, longBirthColor, 0);
            cell->lastColor = longBirthColor;
            break;
        case Cell::LONG_DIE :
            fillArea(cell->x0, cell->y0, cell->lastColor, longDieColor, 0);
            cell->lastColor = longDieColor;
            break;
        case Cell::RECENTLY_DIE :
            fillArea(cell->x0, cell->y0, cell->lastColor, recentlyDieColor, 0);
            cell->lastColor = recentlyDieColor;
            break;
        case Cell::DIE :        
            fillArea(cell->x0, cell->y0, cell->lastColor, backgroundColor, 0);
            cell->lastColor = backgroundColor;
            break;
        default:
            fillArea(cell->x0, cell->y0, defaultWidgetColor, backgroundColor, 0);
            cell->lastColor = backgroundColor;
        }

    if (displayImpact){
        drawText(QString::number(cell->getImpact()), cell->x0, cell->y0, gameLogic->k/2, borderColor);
    }
}



void ImageWidget::setHexagonColored(int mx, int my)
{
    bool draw;
    int i, j, next_index;
    for (j = 0; j < gameLogic->curState.size(); ++j){
        draw = true;

        double cx = mx - gameLogic->curState[j].x0;
        double cy = my - gameLogic->curState[j].y0;

        for (i = 0; i < 6; ++i){
            if (i != 5){
                next_index = i+1;
            } else {
                next_index = 0;
            }
            double bx = round(gameLogic->curState[j].vert[i].first + gameLogic->curState[j].vert[next_index].first)/2.0;
            double by = round(gameLogic->curState[j].vert[i].second + gameLogic->curState[j].vert[next_index].second)/2.0;

            //proection
            double scalar = (cx*bx + cy*by) / (bx*bx + by*by);

            if (abs(scalar) >= 1) {
                draw = false;
                break;
            }
        }

        if (draw){
            if (gameLogic->mode == GameLogic::XOR_MODE){
                if (gameLogic->curState[j].getIsAlive()){
                    gameLogic->curState[j].setIsAlive(false);
                    gameLogic->calculateImpacts();
                    gameLogic->curState[j].setState(Cell::DIE);
                    return;
                }
            }
            gameLogic->curState[j].setIsAlive(true);
            gameLogic->calculateImpacts();
            gameLogic->curState[j].setState(Cell::RECENTLY_BIRTH);
            return;
        }
    }
}

void ImageWidget::createHexagonVertices(Cell* h)
{
    int i;
    double x, y;
    h->vert.clear();
    for (i = 0; i < 6; ++i){
        x = round(gameLogic->k * cos(PI * i/3 + PI/6));
        y = round(gameLogic->k * sin(PI * i/3 + PI/6));
        h->vert.push_back(pair<int,int> (x, y));
    }
}


void ImageWidget::setPixelColor(int x, int y, QColor color)
{
    int i = x + y*width();
    i *= 4;
    bits[i] = color.blue();
    bits[i + 1] = color.green();
    bits[i + 2] = color.red();
    bits[i + 3] = color.alpha();

    /*uchar *ptr = bits;
    int i;
    int w = width() * 4;
    for (i = 0; i < w; ++i){
        ptr += y;
    }
    x*=4;
    ptr += x;
    *(ptr + 2) = color.red();
    *(ptr + 1) = color.green();
    *ptr = color.blue();
    *(ptr + 3) = color.alpha();*/
}

QColor ImageWidget::pixelColor(int x, int y)
{
    int i = x + y*width();
    i *= 4;
    return QColor(bits[i + 2], bits[i + 1], bits[i], bits[i + 3]);
    /*uchar *ptr = bits;
    int i;
    int w = width() * 4;
    for (i = 0; i < w; ++i){
        ptr += y;
    }
    x*=4;
    ptr += x;
    return QColor(*(ptr+2), *(ptr + 1), *ptr, *(ptr + 3));*/
}


//isOct = 0 : 4-connected area
//isOct = 1 : 8-connected area
void ImageWidget::fillArea(int x0, int y0, QColor lastColor, QColor newColor, int isOct)
{
    if (newColor == lastColor || (isOct != 0 && isOct != 1)){
        return;
    }

    if (pixelColor(x0, y0) != lastColor) return;

    int seedY, i;
    stack<Span> stack;
    Span curSpan, sp;

    Span firstSpan = getSpan(x0, y0, lastColor);
    stack.push(firstSpan);

    while (!stack.empty()){
        curSpan = stack.top();

        stack.pop();
        seedY = curSpan.y;
        drawSpan(curSpan, newColor);

        for (i = curSpan.left - isOct; i <= curSpan.right + isOct; ++i){
            if (i < 0) continue;
            if (pixelColor(i, seedY+1) == lastColor){
                sp = getSpan(i, seedY+1, lastColor);                
                stack.push(sp);
                i = sp.right;
            }
        }
        for (i = curSpan.left - isOct; i <= curSpan.right + isOct; ++i){
            if (i < 0) continue;
            if (seedY > 0){
                if (pixelColor(i, seedY-1) == lastColor){
                    sp = getSpan(i, seedY-1, lastColor);
                    stack.push(sp);
                    i = sp.right;
                }
            }
        }
    }
}

void ImageWidget::drawSpan(ImageWidget::Span s, QColor color)
{
    int shiftY = s.y*width();
    int i = 4*(s.left + shiftY);
    int lim = 4*(s.right + shiftY);
    uchar* ptr;
    for (; i <= lim; i += 4){
        ptr = &bits[i];
        *(ptr + 2) = color.red();
        *(ptr + 1) = color.green();
        *ptr = color.blue();
        *(ptr + 3) = color.alpha();
    }
}

void ImageWidget::drawField()
{
    for (auto cell : gameLogic->curState){
        drawHexagonLines(&cell);
        fillCell(&cell);
    }
}

void ImageWidget::createHexagonField(int m, int n)
{
    gameLogic->curState.clear();
    int curY = static_cast<int>(round(sqrt(3)/2*gameLogic->k)); // game X,Y
    int r = curY;
    int stepY = 2*r;
    int startY = curY;
    int curX = gameLogic->k;
    int i, j, lim = m;

    for (i = 0; i < n; ++i){
        curY = startY + r * (i % 2);
        lim = m - i % 2;
        for (j = 0; j < lim; ++j){
            Cell hexagon(curY, curX, i, j); //coords
            createHexagonVertices(&hexagon);
            gameLogic->curState.push_back(hexagon);
            curY += stepY;
        }
        curX += 1.5 * gameLogic->k;
    }
}

void ImageWidget::changeFieldSize()
{
    vector<Cell> newState;
    int i, j, c, x1;
    int m = gameLogic->m;
    int n = gameLogic->n;
    int mm = gameLogic->newM;
    int nn = gameLogic->newN;
    int curY = static_cast<int>(round(sqrt(3)/2*gameLogic->k)); // game X,Y
    int r = curY;
    int stepY = 2*r;
    int startY = curY;
    int curX = gameLogic->k;

    // coords
    for (i = 0; i < nn; ++i){
        curY = startY + r * (i % 2);
        for (j = 0; j < mm - i % 2; ++j){

            if (j  >=  m - i % 2 || i >= n){
                Cell newCell (curY, curX, i, j);
                createHexagonVertices(&newCell);
                newState.push_back(newCell);
            } else {
                c = floor(i/2);
                x1 = (c + i % 2) * m + c * (m - 1);
                Cell newCell(gameLogic->curState[x1+j]);
                createHexagonVertices(&newCell);
                newCell.x0 = curY;
                newCell.y0 = curX;
                newState.push_back(newCell);
            }
            curY += stepY;
        }

        curX += 1.5 * gameLogic->k;
    }
    gameLogic->curState = newState;
    gameLogic->m = mm;
    gameLogic->n = nn;
}

void ImageWidget::drawLine(int x0, int y0, int x1, int y1, QColor color)
{
    int dx = abs(x0 - x1);
    int dy = abs(y0 - y1);

    if (dx < dy){
        int dirX = x0 < x1 ? 1 : -1;
        int curY = x0;
        int i, error = 0;
        if (y1 > y0){
            for (i = y0; i <= y1; ++i){
                setPixelColor(curY, i, color);
                error += 2*dx;
                if (error >= dy){
                    curY += dirX;
                    error -= 2*dy;
                }
            }
        } else {
            for (i = y0; i >= y1; --i){
                setPixelColor(curY, i, color);
                error += 2*dx;
                if (error >= dy){
                    curY += dirX;
                    error -= 2*dy;
                }
            }
        }
    } else {
        int dirY = y0 < y1 ? 1 : -1;
        int curY = y0;
        int i, error = 0;
        if (x1 > x0){
            for (i = x0; i <= x1; ++i){
                setPixelColor(i, curY, color);
                error += 2*dy;
                if (error >= dx){
                    curY += dirY;
                    error -= 2*dx;
                }
            }
        } else {
            for (i = x0; i >= x1; --i){
                setPixelColor(i, curY, color);
                error += 2*dy;
                if (error >= dx){
                    curY += dirY;
                    error -= 2*dx;
                }
            }
        }
    }
}

ImageWidget::Span ImageWidget::getSpan(int x0, int y0, QColor lastColor)
{

    int x = x0;
    int y = y0;
    Span span;
    span.y = y0;

    while (x >= 0 && pixelColor(x, y) == lastColor) {
        x++;
    }
    span.right = x-1;
    x = x0;

    while (x >= 0 && pixelColor(x, y) == lastColor) {
        x--;
    }
    span.left = x+1;
    return span;
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    if (gameLogic->paramsChanged){
        int r = round(sqrt(3)/2*gameLogic->k);
        int R = gameLogic->k;
        int c = floor(gameLogic->newN/2);
        int h = (c + gameLogic->newN % 2) * 2*R + c*R +R/2 + 2;
        int w = 2+2*(gameLogic->newM)*r;
        setMinimumSize(w, h);
        setMaximumSize(w, h);
        changeFieldSize();
        gameLogic->paramsChanged = false;
    }

    delete image;
    image = new QImage(width(), height(), QImage::Format_ARGB32);
    bits = image->bits();

    //QTime t = QTime::currentTime();

    drawField();

    //drawLine(100, 100, 120, 120, QColor(130, 120, 40, 200));

    //qDebug() << -QTime::currentTime().msecsTo(t);

    p.drawImage(0, 0, *image);
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{

    mousePressed = true;
    int mx = event->x();
    int my = event->y();
    setHexagonColored(mx ,my);
    repaint();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!mousePressed) return;
    int mx = event->x();
    int my = event->y();
    setHexagonColored(mx ,my);
    repaint();
}



