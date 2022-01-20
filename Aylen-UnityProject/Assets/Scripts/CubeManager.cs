using UnityEngine;

namespace ViRTSA.Plugin
{
    public class CubeManager : MonoBehaviour
    {
        [SerializeField]
        private bool followServerTransform = false;

        [SerializeField]
        private int identifier;

        [SerializeField]
        private GameObject setup;    // initial cube position

        // From server data
        private Vector3 currentPosition;
        private Quaternion currentRotations;

        void Update()
        {
            if (followServerTransform)
            {
                transform.localPosition = currentPosition;
                transform.localRotation = currentRotations;
            }
        }

        public void ResetCubeTransform()
        {
            transform.localPosition = setup.transform.localPosition;
            transform.localRotation = Quaternion.Euler(setup.transform.localRotation.eulerAngles);
        }

        public void UpdatePosition(Vector3 pos, Quaternion rot)
        {
            //Debug.Log(pos.x + " " + pos.y + " " + pos.z);
            currentPosition = pos;
            currentRotations = rot;
        }

        public void SetFollowServerTransform(bool value)
        {
            followServerTransform = value;
        }
    }
}