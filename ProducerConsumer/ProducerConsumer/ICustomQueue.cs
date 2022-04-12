using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW1
{
    public interface ICustomQueue<T>
    {
        bool IsEmpty { get; }
        void Enqueue(T value);
        bool TryDequeue(out T value);
    }
}
