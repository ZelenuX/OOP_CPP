package theGame;

import java.awt.Color;
import java.io.IOException;

public interface UserInterface {
    public static class RectangleTypes{
        public static final int BASE = -1;
        public static final int EMPTY = 0;
        public static final int ONE_POINT = 1;
        public static final int TWO_POINTS = 2;
        public static final int THREE_POINTS = 3;
        public static final int FOUR_POINTS = 4;
        public static final int FIVE_POINTS = 5;
        public static final int HIGH_POINTS = 6;
    }
    public static class OvalTypes{
        public static final int EMPTY = 0;
        public static final int REGULAR = 1;
    }
    void drawRectangle(double x, double y, double width, double height, int type);/**x, y - from 0 to 1; (x,y) - rectangle left upper corner*/
    void drawOval(double x, double y, double width, double height, int type);/**x, y - from 0 to 1; (x,y) - oval centre*/
    void update();
    void end(String message) throws IOException;
}
