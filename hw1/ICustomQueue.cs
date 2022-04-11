namespace HW1
{
    public interface ICustomQueue<T>
    {
        bool IsEmpty { get; }
        void Enqueue(T value);
        bool TryDequeue(out T value);
    }
}