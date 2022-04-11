namespace synchronization;

public class Producer
{
    private volatile bool _isLaunched = false;
    
    private readonly Mutex _mutex = new Mutex();
    
    private Thread? _thread;
    
    private readonly int _number;

    public Producer(int number)
    {
        _number = number;
    }

    public bool Run(Channel channel)
    {
        // делаем чтение флага, установку его в true и запуск потока атомарной операцией
        // поэтому мы можем вызывать Run(channel) из разных потоков, функция идемпотентна 
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
                    channel.Send(_number);
                    Thread.Sleep(2000);
                }
            });
            _thread.Name = $"producer_{_number}";
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