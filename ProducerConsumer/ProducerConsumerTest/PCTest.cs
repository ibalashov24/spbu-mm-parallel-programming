using Microsoft.VisualStudio.TestTools.UnitTesting;
using HW1;
using System.Threading;

namespace ProducerConsumerTest
{
    [TestClass]
    public class PCTest
    {
    [TestMethod]
        public void DifferentProducers()
        {
            using var cts = new CancellationTokenSource();
            var queueWithLock = new QueueWithLock<int>();
            var a = new Producer<int>(queueWithLock, 1, null, cts.Token);
            var b = new Producer<int>(queueWithLock, 2, null, cts.Token);
            Assert.AreNotEqual(a.Id, b.Id);
            cts.Cancel();
        }

        [TestMethod]
        public void NoConsumers()
        {
            using var cts = new CancellationTokenSource();
            var queueWithLock = new QueueWithLock<int>();
            var a = new Producer<int>(queueWithLock, 1, null, cts.Token);
            Thread.Sleep(2000);
            Assert.IsFalse(queueWithLock.IsEmpty);
            cts.Cancel();
        }

        [TestMethod]
        public void NoProducers()
        {
            using var cts = new CancellationTokenSource();
            var queueWithLock = new QueueWithLock<int>();
            var a = new Consumer<int>(queueWithLock, null, cts.Token);
            Thread.Sleep(2000);
            Assert.IsTrue(queueWithLock.IsEmpty);
            cts.Cancel();
        }

        [TestMethod]
        public void StopProducer()
        {
            using var pcts = new CancellationTokenSource();
            using var ccts = new CancellationTokenSource();
            var queueWithLock = new QueueWithLock<int>();
            var a = new Producer<int>(queueWithLock, 1, null, pcts.Token, 500);
            var b = new Consumer<int>(queueWithLock, null, ccts.Token, 1000);

            Thread.Sleep(2000);

            pcts.Cancel();

            Thread.Sleep(10000);
            ccts.Cancel();

            Assert.IsTrue(queueWithLock.IsEmpty);
        }
    }
}