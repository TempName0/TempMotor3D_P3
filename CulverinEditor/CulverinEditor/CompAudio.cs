﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CulverinEditor
{
    public class CompAudio : CulverinBehaviour
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PlayEvent(string name);
    }
}
