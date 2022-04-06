using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Task1_Sync
{
    /// <summary>
    /// Класс расширений типа List<T>.
    /// </summary>
    public static class ListExtension
    {
        /// <summary>
        /// Расширения List<T> методом перевода в строку.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="list"></param>
        /// <returns></returns>
        public static string ListToString<T>(this List<T> list)
        {
            StringBuilder sb = new StringBuilder();

            foreach (T item in list)
            {
                sb.Append(item.ToString()).Append(' ');
            }

            return sb.ToString();
        }
    }
}
