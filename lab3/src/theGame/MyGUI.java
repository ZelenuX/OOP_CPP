package theGame;

import theGame.Resources.ResourceLoader;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.security.InvalidParameterException;

public class MyGUI implements UserInterface {
    private final Controller controller;
    private ResourceLoader resLoader = new ResourceLoader("C:\\Users\\il_ya\\IdeaProjects\\lab3\\src\\theGame\\Resources", "bmp");
    private JFrame mainFrame = new JFrame("TheGame");
    private int width, height;
    private BufferedImage fieldImage;   /**journey of rectangles and ovals: imageGraphics -> fieldImage -> fieldPanel -> mainFrame*/
    private BufferedImage background;
    private Graphics imageGraphics;
    private final JPanel fieldPanel = new JPanel() {
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            g.drawImage(background, 0, 0, null);
            g.drawImage(fieldImage, 0, 0, null);
        }
    };

    private Runnable jobForUIProcess = new Runnable() {
        @Override
        public void run() {
            fieldPanel.repaint();
        }
    };
    public MyGUI(int width, int height, Controller controller) throws IOException {
        if (controller == null)
            throw new InvalidParameterException();
        this.controller = controller;
        if (width < 100 || height < 100 || width > 1280 || height > 1024)
            throw new InvalidParameterException();
        this.width = width;
        this.height = height;

        fieldImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        imageGraphics = fieldImage.getGraphics();
        drawRectangle(0, 0, 1, 1, RectangleTypes.EMPTY);

        background = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        background.getGraphics().drawImage(ImageIO.read(new File("C:\\Users\\il_ya\\IdeaProjects\\lab3\\src\\theGame\\Resources\\background.bmp")), 0, 0, width, height, null);

        fieldPanel.setBounds(0, 0, width, height);

        Button helpButton = new Button("GOD HELP");
        helpButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                controller.godHelp();
            }
        });
        helpButton.setBounds(0, height, width, 50);

        helpButton.addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent keyEvent) {}
            @Override
            public void keyPressed(KeyEvent keyEvent) {
                switch(keyEvent.getKeyCode()){
                    case KeyEvent.VK_RIGHT: case KeyEvent.VK_D:
                        controller.platformMoveRight();
                        break;
                    case KeyEvent.VK_LEFT: case KeyEvent.VK_A:
                        controller.platformMoveLeft();
                        break;
                }
            }
            @Override
            public void keyReleased(KeyEvent keyEvent) {
                switch(keyEvent.getKeyCode()){
                    case KeyEvent.VK_RIGHT: case KeyEvent.VK_D:
                    case KeyEvent.VK_LEFT: case KeyEvent.VK_A:
                        controller.stopPlatform();
                        break;
                }
            }
        });

        mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        mainFrame.add(fieldPanel);
        mainFrame.add(helpButton);
        mainFrame.setSize(width + 14, height + 85);
        mainFrame.setMaximumSize(mainFrame.getSize());
        mainFrame.setMinimumSize(mainFrame.getSize());
        mainFrame.setLayout(null);
        mainFrame.setIconImage(resLoader.loadRectangle(1));
        mainFrame.setVisible(true);
    }

    private static Composite transparentComposite = AlphaComposite.getInstance(AlphaComposite.CLEAR, 0.0f);
    private static Composite nonTransparentComposite = AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f);
    @Override
    public void drawRectangle(double x, double y, double width, double height, int type) throws InvalidParameterException {
        BufferedImage rectImage;
        try {
            switch (type) {
                case RectangleTypes.EMPTY:
                    ((Graphics2D)imageGraphics).setComposite(transparentComposite);
                    imageGraphics.fillRect((int)(x * this.width), (int)(y * this.height),
                            (int)((x + width) * this.width + 0.00001) - (int)(x * this.width), (int)((y + height) * this.height + 0.00001) - (int)(y * this.height));
                    ((Graphics2D)imageGraphics).setComposite(nonTransparentComposite);
                    return;
                case RectangleTypes.ONE_POINT:
                    rectImage = resLoader.loadRectangle(1);
                    break;
                case RectangleTypes.TWO_POINTS:
                    rectImage = resLoader.loadRectangle(2);
                    break;
                case RectangleTypes.THREE_POINTS:
                    rectImage = resLoader.loadRectangle(3);
                    break;
                case RectangleTypes.FOUR_POINTS:
                    rectImage = resLoader.loadRectangle(4);
                    break;
                case RectangleTypes.FIVE_POINTS:
                    rectImage = resLoader.loadRectangle(5);
                    break;
                case RectangleTypes.HIGH_POINTS:
                    rectImage = resLoader.loadRectangle(6);
                    break;
                case RectangleTypes.BASE:
                    rectImage = resLoader.loadRectangle(7);
                    break;
                default:
                    throw new InvalidParameterException();
            }
        }
        catch (IOException e){
            System.err.println("ERROR: Can not read image file.");
            throw new RuntimeException();
        }
        imageGraphics.drawImage(rectImage, (int)(x * this.width), (int)(y * this.height),
                (int)((x + width) * this.width + 0.00001) - (int)(x * this.width), (int)((y + height) * this.height + 0.00001) - (int)(y * this.height), null);
    }
    @Override
    public void drawOval(double x, double y, double width, double height, int type) throws InvalidParameterException {
        switch (type){
            case OvalTypes.EMPTY:
                ((Graphics2D)imageGraphics).setComposite(transparentComposite);
                break;
            case OvalTypes.REGULAR:
                imageGraphics.setColor(Color.BLUE);
                break;
            default:
                throw new InvalidParameterException();
        }
        x -= width / 2;
        y -= height / 2;
        imageGraphics.fillOval((int)(x * this.width), (int)(y * this.height),
                (int)((x + width) * this.width + 0.00001) - (int)(x * this.width), (int)((y + height) * this.height + 0.00001) - (int)(y * this.height));
        if (type == OvalTypes.EMPTY)
            ((Graphics2D)imageGraphics).setComposite(nonTransparentComposite);
    }
    @Override
    public void update() {
        SwingUtilities.invokeLater(jobForUIProcess);
    }
    @Override
    public void end(String message) throws IOException {
        JFrame endFrame = new JFrame();
        endFrame.setBounds((width + 14 - 150) / 2, height / 2, 150, 75);
        endFrame.setIconImage(resLoader.loadRectangle(1));
        endFrame.add(new JLabel(message));
        endFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        endFrame.setVisible(true);
    }
}
