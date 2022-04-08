namespace Task1_Sync;

/// <summary>
/// Класс с volatile списком.
/// </summary>
/// <typeparam name="T"></typeparam>
public class VolatileList<T>
{
    public volatile List<T> ListOfObjects = new List<T>();
}
