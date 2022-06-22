// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using InvalidCSharp;

namespace RefStructsTests
{
    class Program
    {
        static int Main(string[] args)
        {
            RefFieldValidate.Validate_RefStructWithRefField_Load();
            RefFieldValidate.Validate_Create_RefField();
            RefFieldValidate.Validate_Create_RefStructField();
            RefFieldValidate.Validate_Create_TypedReferenceRefField();

            RefLikeValidate.Validate_Casting_Scenarios();
            RefLikeValidate.Validate_RecognizedOpCodeSequences_Scenarios();
            RefLikeValidate.Validate_Inlining_Behavior();

            return 0;
        }
    }

    public class RefFieldValidate
    {
        private ref struct Explicit
        {
            public Span<byte> Bytes;
            public Guid Guid;
        }

        public static void Validate_RefStructWithRefField_Load()
        {
            var t = typeof(WithRefField);
        }

        private static void Create_RefField_Worker(string str, int depth)
        {
            if (depth == 5)
            {
                return;
            }

            WithRefField s = new(ref str);
            string newStr = new(str);

            Create_RefField_Worker(str + $" {depth}", depth + 1);

            if (s.ConfirmFieldInstance(newStr))
            {
                throw new Exception();
            }

            if (!s.ConfirmFieldInstance(str))
            {
                throw new Exception();
            }

        }

        public static void Validate_Create_RefField()
        {
            var str = nameof(Validate_Create_RefField);
            Create_RefField_Worker(str, 1);
        }

        private static void Create_RefStructField_Worker(string str, int depth)
        {
            if (depth == 5)
            {
                return;
            }

            WithRefField s = new(ref str);
            WithRefStructField t = new(ref s);

            Create_RefStructField_Worker(str + $" {depth}", depth + 1);

            if (!t.ConfirmFieldInstance(ref s))
            {
                throw new Exception();
            }

        }

        public static void Validate_Create_RefStructField()
        {
            var str = nameof(Validate_Create_RefStructField);
            Create_RefStructField_Worker(str, 1);
        }

        private static void Create_TypedReferenceRefField_Worker(RefFieldValidate v, int depth)
        {
            if (depth == 5)
            {
                return;
            }

            WithTypedReferenceField<RefFieldValidate> s = new(ref v);

            Create_TypedReferenceRefField_Worker(v, depth + 1);

            if (typeof(RefFieldValidate) != s.GetFieldType())
            {
                throw new Exception();
            }

            if (!s.ConfirmFieldInstance(v))
            {
                throw new Exception();
            }
        }

        public static void Validate_Create_TypedReferenceRefField()
        {
            RefFieldValidate v = new();
            Create_TypedReferenceRefField_Worker(v, 1);
        }
    }

    public class RefLikeValidate
    {
        public static void Validate_Casting_Scenarios()
        {
            // Opcodes that can handle cases naturally for ByRefLike types should fail.
            // Since ByRefLike types can never be boxed, it stands to reason attempting
            // to cast an object to a ByRefLike type will always return null or throw an
            // appropriate exception.

            if (Exec.InstanceOfT(new object()))
            {
                throw new Exception();
            }
        }

        public static void Validate_RecognizedOpCodeSequences_Scenarios()
        {
            if (!(Exec.BoxUnboxAny() && Exec.BoxBranch() && Exec.BoxIsinstUnboxAny() && Exec.BoxIsinstBranch()))
            {
                throw new Exception();
            }

        }

        public static void Validate_Inlining_Behavior()
        {
            // The call here is to ensure that an inlinable method that isn't called
            // due to a branch, see input argument, doesn't break the compilation of the
            // rest of method that is executed.

            if (!Exec.TypeLoadExceptionAvoidsInline(false))
            {
                throw new Exception();
            }
        }
    }
}
