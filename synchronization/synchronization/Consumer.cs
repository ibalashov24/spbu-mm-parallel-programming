namespace synchronization;

public class Consumer
{
    private volatile bool _isLaunched = false;
    
    private readonly Mutex _mutex = new Mutex();
    
    private Thread? _thread;
    
    private readonly int _number;

    public Consumer(int number)
    {
        _number = number;
    }

    public bool Run(Channel channel)
    {
        _mutex.WaitOne();
        try
        {
            if (_isLaunched)
            {
                return false;
            }

            _thread = new Thread(() =>
            {
                while (_isLaunched)
                {
                    Console.WriteLine($"Consumer {_number}: {channel.Recieve()}");
                    Thread.Sleep(2000);
                }
            });
            _thread.Name = $"consumer_{_number}";
            _isLaunched = true;
            _thread.Start();
            return _isLaunched;
        }
        finally
        {
            _mutex.ReleaseMutex();
        }
    }

    public void Stop()
    {
        _isLaunched = false;
    }
}