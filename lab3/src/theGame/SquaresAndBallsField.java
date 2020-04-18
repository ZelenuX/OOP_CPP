package theGame;

import java.io.IOException;
import java.security.InvalidParameterException;
import java.util.*;

public class SquaresAndBallsField implements BallsField {
    /**
     * Upper left corner is (0,0),
     * right upper corner is (cols, 0),
     * left down corner is (0, filledRows + topEmptyRows + bottomEmptyRows).
     * */
    private int cols, topEmptyRows, filledRows, bottomEmptyRows;
    private int points[]; //0 <-> square is destroyed
    private int score = 0, maxScore = 0, numberOfSquares;
    private int getPoints(int filledRow, int col){
        return points[filledRow * cols + col];
    }
    private void setPoints(int filledRow, int col, int value){
        points[filledRow * cols + col] = value;
    } //setPoints(0) destroys square, setPoints(non 0) creates square

    static class Ball{
        double x, y, vx, vy, radius;
        Ball(double x, double y, double vx, double vy, double diameter){
            this.x=  x;
            this.y = y;
            this.vx = vx;
            this.vy = vy;
            this.radius = diameter / 2;
        }
    }
    private final List<Ball> balls = new ArrayList<>();
    private int numberOfBalls = 0;
    private final double maxBallStartSpeed = 1000;

    private double platformX/*left side*/, platformY = 2 /*height above field bottom before constructor call*/;
    private double platformWidth, platformHeight = 0.5;
    private double platformSpeed = 0, platformMaxSpeed = 5;

    private UserInterface userInterface = new UserInterface() {
        @Override
        public void drawRectangle(double x, double y, double width, double height, int type) {}
        @Override
        public void drawOval(double x, double y, double width, double height, int type) {}
        @Override
        public void update() {}
        @Override
        public void end(String message) throws IOException {}
    };  //to avoid NullPointerException
    @Override
    public void setUI(UserInterface userInterface) throws NullPointerException {
        if (userInterface == null)
            throw new NullPointerException();
        this.userInterface = userInterface;
        userInterface.drawRectangle(platformX / cols, platformY / (filledRows + topEmptyRows + bottomEmptyRows),
                platformWidth / cols, platformHeight / (filledRows + topEmptyRows + bottomEmptyRows), UserInterface.RectangleTypes.BASE);
        int rectangleType;
        for (int i = 0; i < filledRows; ++i)
            for (int j = 0; j < cols; ++j) {
                switch (points[i * cols + j]){
                    case 0:
                        rectangleType = UserInterface.RectangleTypes.EMPTY;
                        break;
                    case 1: case 2: case 3: case 4: case 5:
                        rectangleType = UserInterface.RectangleTypes.ONE_POINT + points[i * cols + j] - 1;
                        break;
                    default:
                        rectangleType = UserInterface.RectangleTypes.HIGH_POINTS;
                        break;
                }
                userInterface.drawRectangle((double)j / cols, (double)(topEmptyRows + i) / (filledRows + topEmptyRows + bottomEmptyRows),
                        1.0 / cols, 1.0 / (filledRows + topEmptyRows + bottomEmptyRows), rectangleType);
            }
    }
    @Override
    public void setPlatformSpeed(double newVx) {
        newVx *= cols;
        if (Math.abs(newVx) <= platformMaxSpeed) {
            platformSpeed = newVx;
        }
        else{
            platformSpeed = platformMaxSpeed * Math.signum(newVx);
        }
    }
    @Override
    public double getPlatformMaxSpeed() {
        return (double)platformMaxSpeed / cols;
    }
    @Override
    public void setPlatformMaxSpeed(double vMax) {
        vMax *= cols;
        if (vMax < 0)
            throw new InvalidParameterException();
        platformMaxSpeed = vMax;
    }
    private void initParams(int cols, int topEmptyRows, int filledRows, int bottomEmptyRows){
        this.cols = cols;
        this.filledRows = filledRows;
        this.topEmptyRows = topEmptyRows;
        this.bottomEmptyRows = bottomEmptyRows;

        numberOfSquares = cols * filledRows;
        points = new int[cols * filledRows];

        platformWidth = (double)cols / 5;
        platformX = (cols - platformWidth) / 2;
        platformY = filledRows + topEmptyRows + bottomEmptyRows - platformY;
        platformMaxSpeed *= (double)cols;
    }
    public SquaresAndBallsField(int points[][]/**first index is y (more is higher)*/, int topEmptyRows, int bottomEmptyRows) throws InvalidParameterException {
        if (points[0].length == 0 || topEmptyRows < 0 || points.length == 0 || bottomEmptyRows <= platformY)
            throw new InvalidParameterException();
        initParams(points[0].length, topEmptyRows, points.length, bottomEmptyRows);
        for(int i = 0; i < filledRows; ++i) {
            if (points[i].length != filledRows)
                throw new InvalidParameterException();
            for (int j = 0; j < cols; ++j) {
                setPoints(i, j, points[filledRows - i - 1][j]);
                maxScore += points[filledRows - i - 1][j];
            }
        }
    }
    public SquaresAndBallsField(int cols, int topEmptyRows, int rowPoints[]/**index is more - row is higher*/, int bottomEmptyRows) throws InvalidParameterException {
        if (cols <= 0 || topEmptyRows < 0 || rowPoints.length == 0 || bottomEmptyRows <= platformY)
            throw new InvalidParameterException();
        initParams(cols, topEmptyRows, rowPoints.length, bottomEmptyRows);
        for(int i = 0; i < filledRows; ++i) {
            for (int j = 0; j < cols; ++j)
                setPoints(i, j, rowPoints[filledRows - i - 1]);
            maxScore += cols * rowPoints[filledRows - i - 1];
        }
    }
    public SquaresAndBallsField(int cols, int topEmptyRows, int filledRows, int bottomEmptyRows) throws InvalidParameterException {
        if (cols <= 0 || topEmptyRows < 0 || filledRows <= 0 || bottomEmptyRows <= platformY)
            throw new InvalidParameterException();
        initParams(cols, topEmptyRows, filledRows, bottomEmptyRows);
        for(int i = 0; i < filledRows - 5; ++i) {
            for (int j = 0; j < cols; ++j)
                setPoints(i, j, 6);
            maxScore += 6 * cols;
        }
        for(int i = filledRows - 5; i < filledRows; ++i) {
            for (int j = 0; j < cols; ++j)
                setPoints(i, j, filledRows - i);
            maxScore += (filledRows - i) * cols;
        }
    }
    public SquaresAndBallsField() throws InvalidParameterException{
        this(20,3,5,10);
    }
    @Override
    public void addBall(double x, double y, double vx, double vy, double diameter) throws InvalidParameterException {
        x *= cols; y = (1 - y) * (filledRows + topEmptyRows + bottomEmptyRows);
        vy = - vy;
        if (diameter <= 0 || (vx == 0 && vy == 0) || vx * vx + vy * vy > maxBallStartSpeed * maxBallStartSpeed
                || x - diameter / 2 <= 0 || x + diameter / 2 >= cols
                || y - diameter / 2 <= 0 || (y + diameter / 2 >= topEmptyRows && y - diameter / 2 <= topEmptyRows + filledRows) || y + diameter / 2 >= topEmptyRows + filledRows + bottomEmptyRows)
            throw new InvalidParameterException();
        balls.add(new Ball(x, y, vx, vy, diameter));
        ++numberOfBalls;
    }
    @Override
    public void multiplyBallsSpeed(double multiplier) {
        for (Ball cur : balls) {
            cur.vx *= multiplier;
            cur.vy *= multiplier;
        }
    }
    @Override
    public double lowestBallStartPos() {
        return 1 - (double)platformY / (filledRows + topEmptyRows + bottomEmptyRows);
    }
    @Override
    public double highestBallStartPos() {
        return (double)bottomEmptyRows / (filledRows + topEmptyRows + bottomEmptyRows);
    }
    @Override
    public int getCols() {
        return cols;
    }
    @Override
    public int getRows() {
        return filledRows + topEmptyRows + bottomEmptyRows;
    }
    @Override
    public void nextMoment(double passedSeconds) throws WinException, FailException {
        final double dt = passedSeconds;
        if (platformSpeed > 0 && platformX + platformWidth > cols - 0.5
            || platformSpeed < 0 && platformX < 0.5)
            platformSpeed = 0;
        if (platformSpeed != 0)
            userInterface.drawRectangle(platformX / cols, platformY / (filledRows + topEmptyRows + bottomEmptyRows),
                    platformWidth / cols, platformHeight / (filledRows + topEmptyRows + bottomEmptyRows), UserInterface.RectangleTypes.EMPTY);
        double originalMovingDistance;
        double leastMovingDistance;
        double leastRemainDistance;
        int nearestI, nearestJ;
        double tmp, tmpx, tmpy;
        double curPlatformX = platformX;
        Iterator<Ball> iterator = balls.iterator();
        Ball cur;
        while (iterator.hasNext()){
            cur = iterator.next();
            userInterface.drawOval(cur.x / cols, cur.y / (filledRows + topEmptyRows + bottomEmptyRows),
                    cur.radius * 2 / cols, cur.radius * 2 / (filledRows + topEmptyRows + bottomEmptyRows), UserInterface.OvalTypes.EMPTY);
            originalMovingDistance = dt * Math.sqrt(cur.vx * cur.vx + cur.vy * cur.vy);
            leastMovingDistance = originalMovingDistance;
            leastRemainDistance = 0;
            nearestI = 0; nearestJ = 0; //nearestI < 0 - special values
            for (int iters = 100000; iters > 0; --iters) {
                leastMovingDistance -= leastRemainDistance;
                leastRemainDistance = Double.POSITIVE_INFINITY;
                //checking field sides
                if (collider.collide(0, -1, cols, 1, cur.x, cur.y, cur.radius, cur.vx, cur.vy) < leastRemainDistance){
                    leastRemainDistance = collider.getRemainDistance();
                    nearestI = -1;
                }
                if (collider.collide(0, filledRows + topEmptyRows + bottomEmptyRows, cols, 1, cur.x, cur.y, cur.radius, cur.vx, cur.vy) < leastRemainDistance){
                    leastRemainDistance = collider.getRemainDistance();
                    nearestI = -2;
                }
                if (collider.collide(-1, 0, 1, filledRows + topEmptyRows + bottomEmptyRows, cur.x, cur.y, cur.radius, cur.vx, cur.vy) < leastRemainDistance){
                    leastRemainDistance = collider.getRemainDistance();
                    nearestI = -3;
                }
                if (collider.collide(cols, 0, 1, filledRows + topEmptyRows + bottomEmptyRows, cur.x, cur.y, cur.radius, cur.vx, cur.vy) < leastRemainDistance){
                    leastRemainDistance = collider.getRemainDistance();
                    nearestI = -4;
                }
                //checking platform
                tmp = collider.collide(curPlatformX, platformY, platformWidth, platformHeight, cur.x, cur.y, cur.radius, cur.vx - platformSpeed, cur.vy);
                tmpy = tmp * cur.vy / Math.sqrt((cur.vx  - platformSpeed) * (cur.vx  - platformSpeed) + cur.vy * cur.vy);
                tmpx = tmpy * cur.vx / cur.vy;
                tmp = Math.sqrt(tmpx * tmpx + tmpy * tmpy); //from here tmp - remain distance to platform
                if (tmp < leastRemainDistance) {
                    leastRemainDistance = tmp;
                    nearestI = -5;
                }
                //checking squares
                for (int i = 0; i < filledRows; ++i)
                    for (int j = 0; j < cols; ++j) {
                        if (Math.sqrt((i + topEmptyRows - cur.y) * (i + topEmptyRows - cur.y) + (j - cur.x) * (j - cur.x)) <= leastMovingDistance + cur.radius + 1.43)
                            if (points[i * cols + j] != 0 &&
                                    collider.collide(j, topEmptyRows + i, 1, 1, cur.x, cur.y, cur.radius, cur.vx, cur.vy) < leastRemainDistance) {
                                leastRemainDistance = collider.getRemainDistance();
                                nearestI = i;
                                nearestJ = j;
                            }
                    }
                if (leastRemainDistance < 0)
                    leastRemainDistance = 0.001;
                    //throw new RuntimeException();
                if (leastRemainDistance > leastMovingDistance)
                    break;
                if (nearestI < 0){
                    switch (nearestI){
                        case -1:
                            collider.collide(0, -1, cols, 1, cur.x, cur.y, cur.radius, cur.vx, cur.vy);
                            break;
                        case -2:
                            iterator.remove();
                            cur = null;
                            --numberOfBalls;
                            break;
                        case -3:
                            collider.collide(-1, 0, 1, filledRows + topEmptyRows + bottomEmptyRows, cur.x, cur.y, cur.radius, cur.vx, cur.vy);
                            break;
                        case -4:
                            collider.collide(cols, 0, 1, filledRows + topEmptyRows + bottomEmptyRows, cur.x, cur.y, cur.radius, cur.vx, cur.vy);
                            break;
                        case -5:
                            collider.collide(curPlatformX, platformY, platformWidth, platformHeight, cur.x, cur.y, cur.radius, cur.vx - platformSpeed, cur.vy);
                            break;
                        default:
                            throw new RuntimeException();
                    }
                    if (cur == null)
                        break;
                }
                else {
                    //changing field state
                    score += points[nearestI * cols + nearestJ];
                    points[nearestI * cols + nearestJ] /= 2.5;
                    score -= points[nearestI * cols + nearestJ];
                    if (points[nearestI * cols + nearestJ] == 0)
                        numberOfSquares -= 1;
                    userInterface.drawRectangle((double) nearestJ / cols, (double) (topEmptyRows + nearestI) / (filledRows + topEmptyRows + bottomEmptyRows),
                            1.0 / cols, 1.0 / (filledRows + topEmptyRows + bottomEmptyRows), points[nearestI * cols + nearestJ]);
                    collider.collide(nearestJ, topEmptyRows + nearestI, 1, 1, cur.x - cur.vx, cur.y - cur.vy, cur.radius, cur.vx, cur.vy);
                }
                //changing ball state
                cur.x += cur.vx * dt * leastRemainDistance / originalMovingDistance;
                cur.y += cur.vy * dt * leastRemainDistance / originalMovingDistance;
                collider.countLastNewV();
                cur.vx = collider.getLastVx();
                cur.vy = collider.getLastVy();
                if (nearestI == -5)
                    cur.vx += platformSpeed * 1.25;
                curPlatformX += platformSpeed * dt * leastRemainDistance / originalMovingDistance;
            }
            if (cur == null)
                continue;
            cur.x += cur.vx * dt * leastMovingDistance / originalMovingDistance;
            cur.y += cur.vy * dt * leastMovingDistance / originalMovingDistance;

            userInterface.drawOval(cur.x / cols, cur.y / (filledRows + topEmptyRows + bottomEmptyRows),
                    cur.radius * 2 / cols, cur.radius * 2 / (filledRows + topEmptyRows + bottomEmptyRows), UserInterface.OvalTypes.REGULAR);
        }
        platformX += platformSpeed * dt;
        if (platformSpeed != 0)
            userInterface.drawRectangle(platformX / cols, platformY / (filledRows + topEmptyRows + bottomEmptyRows),
                    platformWidth / cols, platformHeight / (filledRows + topEmptyRows + bottomEmptyRows), UserInterface.RectangleTypes.BASE);
        if (numberOfSquares == 0)
            throw new WinException();
        if (numberOfBalls == 0)
            throw new FailException();
    }
    @Override
    public int numberOfShapes() {
        return numberOfBalls + numberOfSquares;
    }
    @Override
    public double getScore() {
        return (double)score / maxScore;
    }

    RectAndMovingBallCollider collider = new RectAndMovingBallCollider();
}

/**
 * 1) to find remain distance:
 *      double dst = collide(...);
 * 2) to find new speed of ball after collision:
 *      collide(...);
 *      countLastNewV();
 *      double newVx = getLastVx();
 *      double newVy = getLastVy();
 * */
class RectAndMovingBallCollider{
    /**
     * rx - x of rect. left upper corner
     * rw - rect. width
     * bx - x of ball centre
     * br - ball radius
     * bvx - ball x-speed
     */
    private double rx, ry, rw, rh, bx, by, br, bvx, bvy;
    private int lc = 0; /** defines last collision part of rectangle (0 - no collision, from 1 to 4 - dots, 5 and 6 - sides) */
    private double remainDistance = Double.NaN;  /** remain distance before collision of ball and rectangle previously passed to collide(...) */
    double getRemainDistance(){
        return remainDistance;
    }

    /** sets and returns remainDistance, sets lc */
    double collide(double rectX, double rectY, double rectWidth, double rectHeight,
                   double ballX, double ballY, double ballRadius, double ballVx, double ballVy) throws InvalidParameterException {
        if (ballVx == 0 && ballVy == 0)
            throw new InvalidParameterException();
        rx = rectX; ry = rectY; rw = rectWidth; rh = rectHeight; bx = ballX; by = ballY; br = ballRadius; bvx = ballVx; bvy = ballVy;
        double d1, d2, d3, d4, dHorizontalLine, dVerticalLine, x, y; //d1, d2, d3, d4 - distances to dots, x,y - intersection coords with horizontal and vertical lines
        dHorizontalLine = Double.POSITIVE_INFINITY;
        dVerticalLine = Double.POSITIVE_INFINITY;
        if (bvx > 0){
            if (bvy > 0) {
                d1 = remainDstToDot(rx, ry);
                d2 = remainDstToDot(rx + rw, ry);
                d3 = remainDstToDot(rx, ry + rh);
                d4 = Double.POSITIVE_INFINITY;
                x = collideWithHorizontalLine(ry);
                y = collideWithVerticalLine(rx);
                if (x > rx && x < rx + rw){
                    dHorizontalLine = remainDstToHorizontalLine(ry);
                }
                if (y > ry && y < ry + rh){
                    dVerticalLine = remainDstToVerticalLine(rx);
                }
            }
            else{
                d1 = remainDstToDot(rx, ry);
                d2 = Double.POSITIVE_INFINITY;
                d3 = remainDstToDot(rx, ry + rh);
                d4 = remainDstToDot(rx + rw, ry + rh);
                x = collideWithHorizontalLine(ry + rh);
                y = collideWithVerticalLine(rx);
                if (x > rx && x < rx + rw){
                    dHorizontalLine = remainDstToHorizontalLine(ry + rh);
                }
                if (y > ry && y < ry + rh){
                    dVerticalLine = remainDstToVerticalLine(rx);
                }
            }
        }
        else{
            if (bvy > 0) {
                d1 = remainDstToDot(rx, ry);
                d2 = remainDstToDot(rx + rw, ry);
                d3 = Double.POSITIVE_INFINITY;
                d4 = remainDstToDot(rx + rw, ry + rh);
                x = collideWithHorizontalLine(ry);
                y = collideWithVerticalLine(rx + rw);
                if (x > rx && x < rx + rw){
                    dHorizontalLine = remainDstToHorizontalLine(ry);
                }
                if (y > ry && y < ry + rh){
                    dVerticalLine = remainDstToVerticalLine(rx + rw);
                }
            }
            else{
                d1 = Double.POSITIVE_INFINITY;
                d2 = remainDstToDot(rx + rw, ry);
                d3 = remainDstToDot(rx, ry + rh);
                d4 = remainDstToDot(rx + rw, ry + rh);
                x = collideWithHorizontalLine(ry + rh);
                y = collideWithVerticalLine(rx + rw);
                if (x > rx && x < rx + rw){
                    dHorizontalLine = remainDstToHorizontalLine(ry + rh);
                }
                if (y > ry && y < ry + rh){
                    dVerticalLine = remainDstToVerticalLine(rx + rw);
                }
            }
        }
        lc = numberOfMin(d1, d2, d3, d4, dHorizontalLine, dVerticalLine);
        switch (lc){
            case 1:
                remainDistance = d1;
                break;
            case 2:
                remainDistance = d2;
                break;
            case 3:
                remainDistance = d3;
                break;
            case 4:
                remainDistance = d4;
                break;
            case 5:
                remainDistance = dHorizontalLine;
                break;
            case 6:
                remainDistance = dVerticalLine;
                break;
            default:
                remainDistance = Double.NaN;
        }
        if (remainDistance == Double.POSITIVE_INFINITY)
            lc = 0;
        return remainDistance;
    }
    /** sets bvx and bvy as they must be after collision */
    void countLastNewV(){
        if (lc == 0)
            return;
        double x, y; //corner coords if ball collides with corner
        switch (lc){
            case 1:
                x = rx;
                y = ry;
                break;
            case 2:
                x = rx + rw;
                y = ry;
                break;
            case 3:
                x = rx;
                y = ry + rh;
                break;
            case 4:
                x = rx + rw;
                y = ry + rh;
                break;
            case 5:
                bvy = -bvy;
                return;
            case 6:
                bvx = -bvx;
                return;
            default:
                throw new RuntimeException();
        }
        double hx, hy;  //now h - vector from ball centre to end of normal vector from dot to ray (|h| == remainDistance)
        double absBv = Math.sqrt(bvx * bvx + bvy * bvy);    //abs of current ball speed
        hx = remainDistance * bvx / absBv;
        hy = remainDistance * bvy / absBv;
        hx = bx - x + hx;   //from here h = vector from dot to ball centre at the moment of collision
        hy = by - y + hy;
        double koef = 2 * (bvx * hx + bvy * hy) / (hx * hx + hy * hy);
        bvx -= koef * hx;
        bvy -= koef * hy;
    }
    double getLastVx(){
        return bvx;
    }
    double getLastVy(){
        return bvy;
    }

    /** returns remain distance for ball before collision with dot */
    private double remainDstToDot(double x, double y){
        double ax, ay; //a - vector from ball centre to dot
        ax = x - bx; ay = y - by;
        if (ax * ax + ay * ay <= br * br)
            return Double.NEGATIVE_INFINITY; //dot is already in ball
        double vecMulAV = ax * bvx + ay * bvy;
        if (vecMulAV <= 0)
            return Double.POSITIVE_INFINITY; //dot is behind the ball
        double sqrC = vecMulAV * vecMulAV / (bvx * bvx + bvy * bvy); //sqrC -  sqr distance from ball centre to end of normal vector from dot to ray
        double sqrD = (ax * ax + ay * ay) - sqrC; //sqrD - sqr distance from dot to ray
        if (sqrD >= br * br)
            return Double.POSITIVE_INFINITY; //ball and dot don't collide
        return Math.sqrt(sqrC) - Math.sqrt(br * br - sqrD);
    }
    /** returns remain distance for ball before collision with horizontal line */
    private double remainDstToHorizontalLine(double y){
        if (Math.abs(y - by) < br)
            return Double.NEGATIVE_INFINITY; //line already intersects ball
        if (bvy / (y - by) <= 0)
            return Double.POSITIVE_INFINITY; //line is behind ball
        return (Math.abs(y - by) - br) * Math.sqrt(1 + (bvx / bvy) * (bvx / bvy));
    }
    /** returns remain distance for ball before collision with vertical line */
    private double remainDstToVerticalLine(double x){
        if (Math.abs(x - bx) < br)
            return Double.NEGATIVE_INFINITY; //line already intersects ball
        if (bvx / (x - bx) <= 0)
            return Double.POSITIVE_INFINITY; //line is behind ball
        return (Math.abs(x - bx) - br) * Math.sqrt(1 + (bvy / bvx) * (bvy / bvx));
    }
    /** returns x of collision point */
    private double collideWithHorizontalLine(double y){
        if (Math.abs(y - by) < br)
            return bx; //line already intersects ball
        if (bvy / (y - by) <= 0)
            return bx; //line is behind ball
        return bx + (Math.abs(y - by) - br) * bvx / Math.abs(bvy);
    }
    /** returns y of collision point */
    private double collideWithVerticalLine(double x){
        if (Math.abs(x - bx) < br)
            return by; //line already intersects ball
        if (bvx / (x - bx) <= 0)
            return by; //line is behind ball
        return by + (Math.abs(x - bx) - br) * bvy / Math.abs(bvx);
    }
    /** returns number of min argument (from 1) */
    private int numberOfMin(double a1, double a2, double a3, double a4, double a5, double a6){
        int res = 1;
        double tmp = a1;
        if (a2 < tmp){
            res = 2;
            tmp = a2;
        }
        if (a3 < tmp){
            res = 3;
            tmp = a3;
        }
        if (a4 < tmp){
            res = 4;
            tmp = a4;
        }
        if (a5 < tmp){
            res = 5;
            tmp = a5;
        }
        if (a6 < tmp){
            res = 6;
            tmp = a6;
        }
        return res;
    }
}
