using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class ToolTip : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
{
    public string tooltipText = "";
    public GameObject tooltip;
    public Canvas myCanvas;
    public bool showAtRight;

    public void OnPointerEnter(PointerEventData eventData)
    {
        // get mouse position in canvas
        Vector2 pos;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(myCanvas.transform as RectTransform, Input.mousePosition, myCanvas.worldCamera, out pos);

        // set text of toolpit, render it visible and repositionate it on mouse position
        tooltip.transform.position = myCanvas.transform.TransformPoint(pos) + new Vector3((showAtRight ? 1 : -1) * 10, -3, 0);
        tooltip.GetComponentInChildren<Text>().text = tooltipText;
        tooltip.SetActive(true);
    }

    public void OnPointerExit(PointerEventData eventData)
    {
        // hide tooltip
        tooltip.GetComponentInChildren<Text>().text = "";
        tooltip.SetActive(false);
    }
}
