using UnityEngine;

namespace ViRTSA.Plugin
{
    public class ForcepsManager : MonoBehaviour
    {
        public bool show = false;

        private Fingerinfo Finger;   // fix structure
        private Fingerinfo Thumb;    // mov structure
        private Quaternion CurrentRotation;

        public GameObject HapticPointFingerPosition;
        public GameObject HapticPointThumbPosition;


        void Start()
        {
            Finger = new Fingerinfo();
            Thumb = new Fingerinfo();
        }

        void Update()
        {
            if (show)
            {
                HapticPointFingerPosition.transform.localPosition = Finger.HapticPointPosition;
                HapticPointFingerPosition.transform.localRotation = CurrentRotation;

                HapticPointThumbPosition.transform.localPosition = Thumb.HapticPointPosition;
                HapticPointThumbPosition.transform.localRotation = CurrentRotation;
            }
        }

        public void UpdatePositions(ForcepsInfo forcep)
        {
            Finger = forcep.Finger;
            Thumb = forcep.Thumb;
            CurrentRotation = forcep.CurrentRotation;
        }
    }
}
