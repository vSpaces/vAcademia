using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UI
{
    public class TimeInterval
    {
        public double IntervalStart = 0;
        public double IntervalEnd = 0;

        public TimeInterval()
        {
            IntervalStart = 0;
            IntervalEnd = 0;
        }

        public TimeInterval( double adIntervalStart, double adIntervalEnd)
        {
            IntervalStart = adIntervalStart;
            IntervalEnd = adIntervalEnd;
        }

        public double Duration
        {
            get
            {
                return IntervalEnd - IntervalStart;
            }
        }
    }
}
