namespace synchronization;

public class Channel
{
    private readonly Mutex _mutex = new Mutex();
    
    private readonly Queue<int> _queue = new Queue<int>();

    public void Send(int i)
    {
        _mutex.WaitOne();
        try
        {
            _queue.Enqueue(i);
        }
        finally
        {
            _mutex.ReleaseMutex();
        }
    }

    public int Recieve()
    {
        while (true)
        {
            _mutex.WaitOne();
            try
            {
                if (_queue.TryDequeue(out var result))
                {
                    return result;
                }
            }
            finally
            {
                _mutex.ReleaseMutex();
            }
            Thread.Sleep(0);
        }
    }
}