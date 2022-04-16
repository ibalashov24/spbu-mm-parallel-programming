import java.util.*;
import java.util.concurrent.ConcurrentHashMap;


public class Synchronization {

    //任务队列taskQueue
    private static volatile List<String> taskQueue = new ArrayList<>(1000);
    private static volatile Map<String,Boolean> map = new ConcurrentHashMap<>();


    private synchronized void putValue(int value) {

        Thread t = Thread.currentThread();
        String name = t.getName();

        System.out.println(name + " " + "Produced: " + value);
        taskQueue.add(String.valueOf(value));


    }

    private void getValue() {
        synchronized (Synchronization.class) {

            if(taskQueue.size()!=0){


                Thread t = Thread.currentThread();
                String name = t.getName();

                String value = taskQueue.get(count-1);
                if(map.get(value) == null) {
                    System.out.println(name + " " + "Consumed: " + value);
                }
                map.put(value,true);
            }

        }
    }


    //生产任务数
    private static volatile int count = 0;

    //消费者线程ConThread
    private static class ConThread extends Thread{

        boolean run = true;

        public void exit(){
            run = false;
        }

        @Override
        public void run() {
            super.run();

            while (run){

                new Synchronization().getValue();

                try {
                    sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }



            }
        }
    }

    //生产者线程ProThread
    private static class ProThread extends Thread{

        boolean run = true;

        public void exit(){
            run = false;
        }

        @Override
        public void run() {
            super.run();
            while (count<1000&&run) {

                //锁住公共资源
                synchronized (taskQueue) {
                    new Synchronization().putValue(count);
                    count++;
                }
                try {
                    sleep(200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

            }
        }
    }

    //创建生产者createPro
    public static void createPro(int num){

        for (int i = 0; i < num; i++) {
            new ProThread().start();
        }

    }

    //创建消费者createCon
    public static void createCon(int num){

        for (int i = 0; i < num; i++) {
            new ConThread().start();
        }

    }

    //程序入口
    public static void main(String []args)
    {

        createPro(2);
        createCon(2);

        while (true) {

            Scanner scanner = new Scanner(System.in);
            if(scanner.hasNext()){
                System.exit(0);
            }

        }
    }

}
