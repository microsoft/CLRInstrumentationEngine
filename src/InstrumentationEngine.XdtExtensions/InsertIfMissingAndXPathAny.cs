// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Microsoft.Web.XmlTransform;

namespace Microsoft.InstrumentationEngine.XdtExtensions
{
    /// <summary>
    /// Transformation which inserts node only if Locator returned nothing, and XPath returned any Node.
    /// </summary>
    public sealed class InsertIfMissingAndXPathAny : Transform
    {
        /// <inheritdoc />
        public InsertIfMissingAndXPathAny() :
            base(TransformFlags.UseParentAsTargetNode,
                 MissingTargetMessage.None)
        {
        }

        /// <inheritdoc />
        protected override void Apply()
        {
            if (this.Arguments == null || this.Arguments.Count != 1)
            {
                throw new ArgumentException("InsertIfMissingAndXPath expected one argument which is an XPath expression");
            }

            if (this.TargetChildNodes == null || this.TargetChildNodes.Count == 0)
            {
                var expandedXPath = Environment.ExpandEnvironmentVariables(this.Arguments[0]);
                var xpath = Utils.DecodeUnicodeChars(expandedXPath);
                if (this.TargetNode.SelectSingleNode(xpath) != null)
                {
                    this.TargetNode.AppendChild(this.TransformNode);
                }
            }
        }
    }
}