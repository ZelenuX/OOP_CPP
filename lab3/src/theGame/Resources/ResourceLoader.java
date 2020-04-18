package theGame.Resources;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;

public class ResourceLoader {
    String resourceFolder;
    String imageFileType;
    HashMap<Integer, BufferedImage> balls = new HashMap<>();
    HashMap<Integer, BufferedImage> rectangles = new HashMap<>();
    HashMap<Integer, BufferedImage> resources = new HashMap<>();
    public ResourceLoader(String resourceFolder, String imageFileType){
        this.resourceFolder = resourceFolder;
        this.imageFileType = imageFileType;
    }
    public BufferedImage loadBall(int number) throws IOException {
        if (balls.containsKey(number))
            return balls.get(number);
        BufferedImage img = ImageIO.read(new File(resourceFolder + "\\ball" + number + '.' + imageFileType));
        balls.put(number, img);
        return img;
    }
    public BufferedImage loadRectangle(int number) throws IOException {
        if (rectangles.containsKey(number))
            return rectangles.get(number);
        BufferedImage img = ImageIO.read(new File(resourceFolder + "\\rect" + number + "." + imageFileType));
        rectangles.put(number, img);
        return img;
    }
    public BufferedImage loadResource(int number) throws IOException {
        if (resources.containsKey(number))
            return resources.get(number);
        BufferedImage img = ImageIO.read(new File(resourceFolder + "\\res" + number + '.' + imageFileType));
        resources.put(number, img);
        return img;
    }
}
