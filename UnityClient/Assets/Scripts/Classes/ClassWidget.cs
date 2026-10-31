using UnityEngine;
using UnityEngine.UI;

public class ClassWidget : MonoBehaviour
{
    public Text classNameText;
    public Text classDescriptionText;
    public Image classColorImage;
    public Button selectClassButton;
    
    private ClassData currentClass;
    
    void Start()
    {
        selectClassButton.onClick.AddListener(SelectClass);
    }
    
    public void ShowClass(ClassData classData)
    {
        currentClass = classData;
        classNameText.text = classData.nameRu;
        // classColorImage.color = classData.classColor; // Would need to add color field
        classDescriptionText.text = classData.descriptionRu;
        gameObject.SetActive(true);
    }
    
    void SelectClass()
    {
        // ServerManager.SelectClass(currentClass.id);
        gameObject.SetActive(true);
    }
}