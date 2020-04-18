package com.company;
import theGame.*;

import java.io.IOException;

public class Main {

    public static void main(String[] args) {
        final int sleepMs = 5;
        int k = 1;
        BallsField field = new SquaresAndBallsField(40 * k, 6 * k, 8 * k, 22 * k);
        Controller controller = new Controller(field);
        UserInterface userInterface;
        try {
            userInterface = new MyGUI(20*40, 18*40, controller);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        field.setUI(userInterface);
        try {
            while (true) {
                controller.nextMoment((double) sleepMs / 1000);
                userInterface.update();
                Thread.sleep(sleepMs);
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (WinException e){
            userInterface.update();
            try {
                userInterface.end("WIN");
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        } catch (FailException e){
            userInterface.update();
            try {
                userInterface.end("FAIL(" + (double)(int)(controller.getScore() * 100 * 1000) / 1000 + "%)");
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }
}
