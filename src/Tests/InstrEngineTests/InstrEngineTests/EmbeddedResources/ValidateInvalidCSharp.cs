// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using InvalidCSharp;

namespace ValidateInvalidCSharp
{
    class Validate
    {
        static int error = 0;

        static int Main(string[] args)
        {
            Validate_Invalid_RefField_Fails();
            Validate_RefStructWithRefField_Load();
            Validate_Create_RefField();
            Validate_Create_RefStructField();
            Validate_Create_TypedReferenceRefField();

            return error; // If everything works as expected, error should be 0
        }

        [StructLayout(LayoutKind.Explicit)]
        private ref struct Explicit
        {
            [FieldOffset(0)] public Span<byte> Bytes;
            [FieldOffset(0)] public Guid Guid;
        }

        private static void Validate_Invalid_RefField_Fails()
        {
            Console.WriteLine($"{nameof(Validate_Invalid_RefField_Fails)}...");

            int errorCount = 0;
            const int expectedErrorCount = 6;

            try
            {
                var t = typeof(InvalidStructWithRefField);
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                var t = typeof(InvalidRefFieldAlignment);
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                var t = typeof(InvalidObjectRefRefFieldOverlap);
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                var t = new IntPtrRefFieldOverlap()
                {
                    Field = IntPtr.Zero
                };

                var tFieldString = t.Field.ToString();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                var t = new IntPtrOverlapWithInnerFieldType()
                {
                    Field = IntPtr.Zero
                };

                ref var i = ref t.Invalid;
                var iSize = i.Size;
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                var t = new Explicit()
                {
                    Guid = Guid.NewGuid()
                };
                var tLength = t.Bytes.Length;
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            error = errorCount == expectedErrorCount ? error : -1;
        }

        private static void Validate_RefStructWithRefField_Load()
        {
            Console.WriteLine($"{nameof(Validate_RefStructWithRefField_Load)}...");
            var t = typeof(WithRefField);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static void Create_RefField_Worker(string str, int depth)
        {
            if (depth == 5)
            {
                return;
            }

            WithRefField s = new(ref str);
            string newStr = new(str);

            Create_RefField_Worker(str + $" {depth}", depth + 1);

            error = !s.ConfirmFieldInstance(newStr) ? error : -1;
            error = s.ConfirmFieldInstance(str) ? error : -1;
        }

        private static void Validate_Create_RefField()
        {
            var str = nameof(Validate_Create_RefField);
            Console.WriteLine($"{str}...");
            Create_RefField_Worker(str, 1);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static void Create_RefStructField_Worker(string str, int depth)
        {
            if (depth == 5)
            {
                return;
            }

            WithRefField s = new(ref str);
            WithRefStructField t = new(ref s);

            Create_RefStructField_Worker(str + $" {depth}", depth + 1);

            error = t.ConfirmFieldInstance(ref s) ? error : -1;
        }

        private static void Validate_Create_RefStructField()
        {
            var str = nameof(Validate_Create_RefStructField);
            Console.WriteLine($"{str}...");
            Create_RefStructField_Worker(str, 1);
        }

        [MethodImpl(MethodImplOptions.NoInlining)]
        private static void Create_TypedReferenceRefField_Worker(Validate v, int depth)
        {
            if (depth == 5)
            {
                return;
            }

            WithTypedReferenceField<Validate> s = new(ref v);

            Create_TypedReferenceRefField_Worker(v, depth + 1);

            error = typeof(Validate) == s.GetFieldType() ? error : -1;
            error = s.ConfirmFieldInstance(v) ? error : -1;
        }

        private static void Validate_Create_TypedReferenceRefField()
        {
            Console.WriteLine($"{nameof(Validate_Create_TypedReferenceRefField)}...");

            Validate v = new();
            Create_TypedReferenceRefField_Worker(v, 1);
        }
    }
}
