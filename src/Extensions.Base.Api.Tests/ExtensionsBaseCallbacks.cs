// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace ProfilerCallbacksTests
{
    using System;
    using System.Globalization;
    using System.Threading;
    using System.Threading.Tasks;

    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using AID = _System.Diagnostics.DebuggerHiddenAttribute;

    [TestClass]
    public class ExtensionsBaseCallbacks
    {
        private readonly int methodId = 25;
        private readonly int numberOfTasks = 250;

        private readonly object thisObj = new object();
        private readonly object returnValue = new object();

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Usage", "CA2201:Do not raise reserved exception types", Justification = "This is specifically used for test code.")]
        private readonly Exception exception = new Exception();

        private CallbackStubIsCalled callbacksStub;

        [TestInitialize]
        public void TestInitialization()
        {
            this.callbacksStub = new CallbackStubIsCalled();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            AID.ApplicationInsights_RemoveCallbacks(methodId, 0);
            AID.ApplicationInsights_RemoveCallbacks(methodId, 1);

            for (int i = 0; i < numberOfTasks; ++ i)
            {
                AID.ApplicationInsights_RemoveCallbacks(i, 0);
            }
        }

        [TestMethod]
        public void ExtensionBaseCallbacksCalled()
        {
            AID.ApplicationInsights_AddCallbacks(methodId,
                callbacksStub.OnBegin0,
                callbacksStub.OnEnd0,
                callbacksStub.OnException0);

            var context = AID.ApplicationInsights_OnBegin(methodId, thisObj);
            Assert.IsTrue(callbacksStub.isOnBeginCalled);

            Assert.AreEqual(null, AID.ApplicationInsights_OnEnd(methodId, context, returnValue, thisObj),
                "return value was not overrided by End callback");
            Assert.IsTrue(callbacksStub.isOnEndCalled);

            AID.ApplicationInsights_OnException(methodId, context, exception, thisObj);
            Assert.IsTrue(callbacksStub.isOnExceptionCalled);
        }

        [TestMethod]
        public void ExtensionBasePrivateCallbacksCalled()
        {
            isOnBeginPrivateCalled = false;
            AID.ApplicationInsights_AddCallbacks(methodId, OnBeginPrivate, null, null);
            var context = AID.ApplicationInsights_OnBegin(methodId, thisObj);
            Assert.IsTrue(isOnBeginPrivateCalled);
        }

        [TestMethod]
        public void ExtensionBaseWrongSignatureMethodWillNotBeCalled()
        {
            AID.ApplicationInsights_AddCallbacks(methodId, callbacksStub.OnBegin1, null, null);
            AID.ApplicationInsights_OnBegin(methodId, thisObj);
            Assert.IsFalse(callbacksStub.IsAnythingCalled);
        }

        [TestMethod]
        public void ExtensionBaseReturnValueSubstituted()
        {
            AID.ApplicationInsights_AddCallbacks(methodId, null, (ctx, retVal) => { return 42; }, null);
            Assert.AreEqual(42, Convert.ToInt32(AID.ApplicationInsights_OnEnd(methodId, null, this.returnValue), CultureInfo.InvariantCulture));
        }

        [TestMethod]
        public void ExtensionBaseReturnValueDefault()
        {
            Assert.AreEqual(this.returnValue, AID.ApplicationInsights_OnEnd(methodId, null, this.returnValue));
        }

        [TestMethod]
        public void ExtensionBaseWrongBeginReturnContext()
        {
            AID.ApplicationInsights_AddCallbacks(methodId, () => { return 42; }, null, null);
            Assert.AreEqual(42, Convert.ToInt32(AID.ApplicationInsights_OnBegin(methodId), CultureInfo.InvariantCulture));
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentException))]
        public void AddThrowsIfSameKeyIsAdded()
        {
            AID.ApplicationInsights_AddCallbacks(1, () => 1, null, null);
            AID.ApplicationInsights_AddCallbacks(1, () => 1, null, null);
        }

        [TestMethod]
        public void ExceptionDoesNotDeadLock()
        {
            AID.ApplicationInsights_AddCallbacks(1, () => 1, null, null);

            try
            {
                AID.ApplicationInsights_AddCallbacks(1, () => 1, null, null);
            }
            catch
            {
            }

            AID.ApplicationInsights_RemoveCallbacks(1, 0); // should not throw concurrency exception
        }

        [TestMethod]
        [Timeout(5000)]
        public void TestMultipleThreadsAddingCallbacks()
        {
            var result = Parallel.For(1, numberOfTasks,
                (i, state) => AID.ApplicationInsights_AddCallbacks(i, () => i, null, null));

            Assert.IsTrue(result.IsCompleted);

            for (int i = 0; i < numberOfTasks; ++i)
            {
                Assert.AreEqual(i, Convert.ToInt32(AID.ApplicationInsights_OnBegin(i), CultureInfo.InvariantCulture));
            }
        }

        [TestMethod]
        [Timeout(5000)]
        public void TestMultipleThreadsAddingAndRemovingCallbacks()
        {
            #region Prepare: add some callbacks first so we have something to delete
            for (int i = 0; i < numberOfTasks; ++i)
            {
                var nonCapturedI = i;
                AID.ApplicationInsights_AddCallbacks(nonCapturedI, () => nonCapturedI, null, null);
            }
            #endregion

            Task[] allTasks = new Task[numberOfTasks * 2];

            for (int i = 0; i < numberOfTasks; ++i)
            {
                int nonCapturedI = i;

                allTasks[nonCapturedI] =
                    Task.Factory.StartNew(
                        () => AID.ApplicationInsights_RemoveCallbacks(nonCapturedI, 0),
                        default(CancellationToken), TaskCreationOptions.None, TaskScheduler.Default);
                allTasks[nonCapturedI + numberOfTasks] =
                    Task.Factory.StartNew(
                        () => AID.ApplicationInsights_AddCallbacks(nonCapturedI + numberOfTasks, () => nonCapturedI + numberOfTasks, null, null),
                        default(CancellationToken), TaskCreationOptions.None, TaskScheduler.Default);
            }

            Task.WaitAll(allTasks);

            for (int i = numberOfTasks; i < 2 * numberOfTasks; ++i)
            {
                Assert.AreEqual(i, Convert.ToInt32(AID.ApplicationInsights_OnBegin(i), CultureInfo.InvariantCulture));
            }
        }

        private bool isOnBeginPrivateCalled;

        private object OnBeginPrivate(object thisObj)
        {
            isOnBeginPrivateCalled = true;
            return null;
        }

        private class CallbackStubIsCalled
        {
            public bool isOnBegin1Called;
            public bool isOnBeginCalled;
            public bool isOnEndCalled;
            public bool isOnExceptionCalled;

            public bool IsAnythingCalled
            {
                get { return isOnBeginCalled || isOnBegin1Called; }
            }

            public object OnBegin0(object thisObj)
            {
                isOnBeginCalled = true;
                return null;
            }

            public object OnBegin1(object thisObj, object arg1)
            {
                isOnBegin1Called = true;
                return null;
            }

            public object OnEnd0(object context, object returnValue, object thisObj)
            {
                isOnEndCalled = true;
                return null;
            }

            public void OnException0(object context, object exception, object thisObj)
            {
                isOnExceptionCalled = true;
            }
        }
    }
}