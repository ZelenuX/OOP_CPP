package theGame;

import java.security.InvalidParameterException;

public class Controller {
    private final BallsField field;
    private double allPassedSeconds = 0;
    private double curPlatformSpeed;
    private int needGodHelps = 0;
    private int platformDirection = 0;  //-1, 0, 1
    public Controller(BallsField field){
        if (field == null)
            throw new InvalidParameterException();
        this.field = field;
        field.addBall(0.5, field.lowestBallStartPos() + (double)1 / field.getRows(),
                -(double)field.getCols() / 3, (double)field.getRows() / 3, 0.75);
        field.setPlatformMaxSpeed(1);
        curPlatformSpeed = 0.5;
    }
    public void nextMoment(double passedSeconds) throws WinException, FailException {
        field.setPlatformSpeed(curPlatformSpeed * platformDirection);
        field.nextMoment(passedSeconds);
        allPassedSeconds += passedSeconds;
        if ((int)allPassedSeconds > (int)(allPassedSeconds - passedSeconds)) {
            field.multiplyBallsSpeed(1.01);
            //curPlatformSpeed *= 1.01;
        }
        int tmp = needGodHelps;
        for (int j = needGodHelps; j > 0; --j)
            for (int i = 1; i < 10; ++i)
                field.addBall(i * 0.1, (field.highestBallStartPos() - field.lowestBallStartPos()) * 0.25 + field.lowestBallStartPos(),
                    curPlatformSpeed * 0.2 * (i - 5) * field.getCols(), curPlatformSpeed * field.getCols(), 0.5);
        needGodHelps -= tmp;
    }
    public void platformMoveRight(){
        platformDirection = 1;
    }
    public void platformMoveLeft(){
        platformDirection = -1;
    }
    public void stopPlatform(){
        platformDirection = 0;
    }
    public void godHelp(){
        ++needGodHelps;
    }
    public double getScore(){
        return field.getScore();
    }
}
