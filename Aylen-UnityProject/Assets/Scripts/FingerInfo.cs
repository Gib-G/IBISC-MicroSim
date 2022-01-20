using UnityEngine;

namespace ViRTSA.Plugin
{
    public class Fingerinfo
    {
        public Vector3 HapticPointPosition;
        public float HapticPointAngle;
        public float MetaProxyAngle;

        public Fingerinfo() { }

        public Fingerinfo(Vector3 hp, float ha, float ma)
        {
            HapticPointPosition = hp;
            HapticPointAngle = ha;
            MetaProxyAngle = ma;
        }
    }
}
