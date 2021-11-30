import java.util.concurrent.Semaphore;
public class TrafficController {

    private Semaphore semLeft;
    private Semaphore semRight;
    private Semaphore bridge;
    
    // How many cars are coming in each direction
    private int noLeft;
    private int noRight;

    public TrafficController() {
        this.noLeft = 0;
        this.noRight = 0;
        this.semLeft = new Semaphore(3, true);
        this.semRight = new Semaphore(3, true);
        this.bridge = new Semaphore(1, true);
    }

    public void enterLeft() {
        try {
           semLeft.acquire();
           if(noLeft == 0){
               // The first car crossing the bridge
               // from the left
               bridge.acquire();
           }
           noLeft++;        
           semLeft.release();
        } catch (InterruptedException e) {
            System.out.println("The thread was interrupted!"); 
        }
    }
    public void enterRight() {
        try {
           semRight.acquire();
           if(noRight == 0){
               bridge.acquire();
           }
           noRight++;
           semRight.release();
        } catch (InterruptedException e) {
            System.out.println("The thread was interrupted!"); 
        }
    }

    
    public void leaveLeft() {
        try {
           semRight.acquire();
           if(noRight == 1){
               bridge.release();
           }
           noRight--;
           semRight.release();
        } catch (InterruptedException e) {
            System.out.println("The thread was interrupted!"); 
        }
    }

    public void leaveRight() {
        try {
           semLeft.acquire();
           if(noLeft == 1){
               bridge.release();
           }
           noLeft--;
           semLeft.release();
        } catch (InterruptedException e) {
            System.out.println("The thread was interrupted!"); 
        }
    }

}
