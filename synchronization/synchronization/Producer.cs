namespace synchronization;

public class Producer
{
    private bool _isLaunched = false;
    private Thread? _thread;
    private readonly int _number;

    public Producer(int number)
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
                channel.Send(_number);
                Thread.Sleep(2000);
            }
        });
        _thread.Name = $"producer_{_number}";
        _isLaunched = true;
        _thread.Start();
        return true;
    }

    public void Stop()
    {
        _isLaunched = false;
    }
}