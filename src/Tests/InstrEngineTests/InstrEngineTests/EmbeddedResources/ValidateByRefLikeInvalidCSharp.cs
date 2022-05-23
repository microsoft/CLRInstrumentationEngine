// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using ByRefLikeInvalidCSharp;

namespace ValidateByRefLikeInvalidCSharp
{
    class Program
    {
        static int error = 0;

        static int Main(string[] args)
        {
            Validate_TypeLoad();
            Validate_Casting_Scenarios();
            Validate_RecognizedOpCodeSequences_Scenarios();
            Validate_InvalidOpCode_Scenarios();
            Validate_Inlining_Behavior();

            return error; // If everything works as expected, error should be 0
        }

        private static void Validate_TypeLoad()
        {
            Console.WriteLine($"{nameof(Validate_TypeLoad)}...");

            Console.WriteLine($" -- Instantiate: {Exec.GenericClass()}");
            Console.WriteLine($" -- Instantiate: {Exec.GenericInterface()}");
            Console.WriteLine($" -- Instantiate: {Exec.GenericValueType()}");
            Console.WriteLine($" -- Instantiate: {Exec.GenericByRefLike()}");

            int errorCount = 0;
            const int expectedErrorCount = 4;

            try
            {
                Exec.GenericClass_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                Exec.GenericInterface_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                Exec.GenericValueType_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                Exec.GenericByRefLike_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            error = errorCount == expectedErrorCount ? error : -1;
        }

        private static void Validate_Casting_Scenarios()
        {
            Console.WriteLine($"{nameof(Validate_Casting_Scenarios)}...");

            // Opcodes that can handle cases naturally for ByRefLike types should fail.
            // Since ByRefLike types can never be boxed, it stands to reason attempting
            // to cast an object to a ByRefLike type will always return null or throw an
            // appropriate exception.

            error = !Exec.InstanceOfT(new object()) ? error : -1;

            int errorCount = 0;
            const int expectedErrorCount = 2;

            try
            {
                Exec.CastToT(new object());
            }
            catch (InvalidCastException)
            {
                errorCount += 1;
            }

            try
            {
                Exec.UnboxToT(new object());
            }
            catch (InvalidCastException)
            {
                errorCount += 1;
            }

            error = errorCount == expectedErrorCount ? error : -1;
        }

        private static void Validate_RecognizedOpCodeSequences_Scenarios()
        {
            Console.WriteLine($"{nameof(Validate_RecognizedOpCodeSequences_Scenarios)}...");

            error = Exec.BoxUnboxAny() ? error : -1;
            error = Exec.BoxBranch() ? error : -1;
            error = Exec.BoxIsinstUnboxAny() ? error : -1;
            error = Exec.BoxIsinstBranch() ? error : -1;
        }

        private static void Validate_InvalidOpCode_Scenarios()
        {
            Console.WriteLine($"{nameof(Validate_InvalidOpCode_Scenarios)}...");

            // These methods uses opcodes that are not able to handle ByRefLike type operands.
            // The TypeLoader prevents these invalid types from being constructed. We rely on
            // the failure to construct these invalid types to block opcode usage.
            int errorCount = 0;
            const int expectedErrorCount = 4;

            try
            {
                Exec.AllocArrayOfT_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                Exec.AllocMultiDimArrayOfT_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            try
            {
                Exec.GenericClassWithStaticField_Invalid();
            }
            catch (TypeLoadException)
            {
                errorCount += 1;
            }

            // Test that explicitly tries to box a ByRefLike type.
            try
            {
                Exec.BoxAsObject();
            }
            catch (InvalidProgramException)
            {
                errorCount += 1;
            }

            error = errorCount == expectedErrorCount ? error : -1;

        }

        private static void Validate_Inlining_Behavior()
        {
            Console.WriteLine($"{nameof(Validate_Inlining_Behavior)}...");

            // The call here is to ensure that an inlinable method that isn't called
            // due to a branch, see input argument, doesn't break the compilation of the
            // rest of method that is executed.
            error = Exec.TypeLoadExceptionAvoidsInline(false) ? error : -1;
        }
    }
}
