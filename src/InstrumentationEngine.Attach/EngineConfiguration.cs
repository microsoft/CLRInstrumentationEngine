// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Xml.Serialization;

namespace Microsoft.InstrumentationEngine
{
    public class InstrumentationEngineConfiguration
    {
        public InstrumentationEngine? InstrumentationEngine { get; set; }

        public InstrumentationMethod[] InstrumentationMethods { get; set; }
    }

    public class InstrumentationEngine
    {
        public Setting[] Settings { get; set; }
    }

    public class Setting
    {
        [XmlAttribute]
        public string Name { get; set; }

        [XmlAttribute]
        public string Value { get; set; }
    }

    public class InstrumentationMethod
    {
    }

    public class AddInstrumentationMethod : InstrumentationMethod
    {
        [XmlAttribute]
        public string ConfigPath { get; set; }

        public Setting[] Settings { get; set; }
    }
}
