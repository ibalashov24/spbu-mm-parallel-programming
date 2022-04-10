namespace synchronization;

public class Consumer
{
    private bool _isLaunched = false;
    private Thread? _thread;
    private readonly int _number;

    public Consumer(int number)
    {
        _number = number;
    }

    public bool Run(Channel channel)
    {
        if (_isLaunched)
        {
            return false;
        }

        _thread = new Thread(() =>
        {
            while (_isLaunched)
            {
                Console.WriteLine(channel.Recieve());
                Thread.Sleep(2000);
            }
        });
        _thread.Name = $"consumer_{_number}";
        _isLaunched = true;
        _thread.Start();
        return true;
    }

    public void Stop()
    {
        _isLaunched = false;
    }
}