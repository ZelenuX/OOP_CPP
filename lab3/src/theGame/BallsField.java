package theGame;

import java.security.InvalidParameterException;

public interface BallsField {
    void addBall(double x, double y, double vx, double vy, double diameter) throws InvalidParameterException;/**x, y - from 0 to 1*/
    void multiplyBallsSpeed(double multiplier);
    double lowestBallStartPos();
    double highestBallStartPos();
    int getCols();
    int getRows();
    void nextMoment(double passedSeconds) throws WinException, FailException;
    void setUI(UserInterface userInterface);
    void setPlatformSpeed(double newVx);/** from -1 to 1 */
    double getPlatformMaxSpeed();
    void setPlatformMaxSpeed(double vMax);
    int numberOfShapes();
    double getScore();/**from 0 to 1*/
}
