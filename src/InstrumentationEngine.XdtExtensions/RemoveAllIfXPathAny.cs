// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Xml;
using Microsoft.Web.XmlTransform;

namespace Microsoft.InstrumentationEngine.XdtExtensions
{
    /// <summary>
    /// Transformation which removes all nodes matched by Locator only if XPath returned any Node.
    /// </summary>
    public sealed class RemoveAllIfXPathAny : Transform
    {
        public RemoveAllIfXPathAny() :
            base(TransformFlags.ApplyTransformToAllTargetNodes,
                 MissingTargetMessage.None)
        {
        }

        /// <inheritdoc />
        protected override void Apply()
        {
            if (this.Arguments == null || this.Arguments.Count != 1)
            {
                throw new ArgumentException("RemoveAllIfXPath expected one argument which is an XPath expression");
            }

            var expandedXPath = Environment.ExpandEnvironmentVariables(this.Arguments[0]);
            var xpath = Utils.DecodeUnicodeChars(expandedXPath);

            // Due to ApplyTransformToAllTargetNodes,
            // The caller will execute this Apply() function against all target nodes that match Locator.
            if (this.TargetNode.SelectSingleNode(xpath) != null)
            {
                XmlNode parentNode = TargetNode.ParentNode;
                parentNode.RemoveChild(TargetNode);
            }
        }
    }
}
