using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// Корневой контроллер клиентского UI на uGUI.
/// Экраны: главное меню -> выбор персонажа -> HUD с панелями
/// (квесты, сумка, профиль, кампании, карта, гильдия, настройки).
/// Разметка строится в рантайме; данные берутся из GameManager.
/// </summary>
public class ClientUIManager : MonoBehaviour
{
    public static ClientUIManager Instance { get; private set; }

    // ---------------------------------------------------------------- theme

    private static readonly Color BG = new Color(0.043f, 0.035f, 0.063f, 1f);
    private static readonly Color PANEL = new Color(0.09f, 0.08f, 0.12f, 0.97f);
    private static readonly Color PANEL_ALT = new Color(0.13f, 0.115f, 0.155f, 1f);
    private static readonly Color BORDER = new Color(0.22f, 0.19f, 0.26f, 1f);
    private static readonly Color ACCENT = new Color(0.72f, 0.22f, 0.28f, 1f);
    private static readonly Color ACCENT_HOVER = new Color(0.82f, 0.28f, 0.35f, 1f);
    private static readonly Color GOLD = new Color(0.937f, 0.80f, 0.55f, 1f);
    private static readonly Color TEXT = new Color(0.93f, 0.92f, 0.95f, 1f);
    private static readonly Color MUTED = new Color(0.62f, 0.60f, 0.66f, 1f);
    private static readonly Color HP = new Color(0.72f, 0.20f, 0.24f, 1f);
    private static readonly Color MP = new Color(0.26f, 0.44f, 0.82f, 1f);
    private static readonly Color XP = new Color(0.79f, 0.64f, 0.33f, 1f);

    private static Font _font;

    // ---------------------------------------------------------------- screens

    private Canvas _canvas;
    private RectTransform _root;

    private GameObject _mainMenu;
    private GameObject _charSelect;
    private GameObject _hud;

    // HUD
    private Text _hudName, _hudLevel, _hudFaction, _hudZone, _hudClock, _hudWeather, _hudGold, _hudSpirit;
    private Text _hpLabel, _mpLabel, _xpLabel;
    private RectTransform _hpFill, _mpFill, _xpFill;

    // chat
    private ScrollRect _chatScroll;
    private RectTransform _chatContent;
    private InputField _chatInput;
    private Text _chatChannel;
    private string _channel = "Общий";
    private static readonly string[] Channels = { "Общий", "Отряд", "Фракция", "Торговля", "Шёпот" };

    // panels
    private GameObject _panelQuests, _panelInventory, _panelProfile, _panelCampaigns,
                     _panelMap, _panelGuild, _panelSettings;
    private RectTransform _questList, _inventoryGrid, _statsList, _campaignList,
                          _guildWarList, _guildAllyList;
    private Text _inventoryCount, _inventoryGold, _inventorySpirit;

    // char select
    private InputField _charName;
    private Dropdown _charRace, _charClass, _charFaction;

    // profile / settings
    private Text _profileName, _profileClass, _profileRace;

    // toast
    private GameObject _toast;
    private Text _toastText;
    private float _toastTimer;

    // ---------------------------------------------------------------- lifecycle

    private void Awake()
    {
        Instance = this;
        _font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
        if (_font == null) _font = Resources.GetBuiltinResource<Font>("Arial.ttf");

        EnsureEventSystem();
        EnsureCamera();
        BuildCanvas();

        BuildMainMenu();
        BuildCharacterSelect();
        BuildHud();
        BuildToast();

        BuildStaticContent();
        ShowMainMenu();
    }

    private void Update()
    {
        if (_toastTimer > 0f)
        {
            _toastTimer -= Time.deltaTime;
            if (_toastTimer <= 0f && _toast != null)
                _toast.SetActive(false);
        }

        if (_hud != null && _hud.activeSelf)
            HandleHotkeys();
    }

    private void HandleHotkeys()
    {
        if (_chatInput != null && _chatInput.isFocused) return;

        if (Input.GetKeyDown(KeyCode.L)) TogglePanel(_panelQuests);
        if (Input.GetKeyDown(KeyCode.I)) TogglePanel(_panelInventory);
        if (Input.GetKeyDown(KeyCode.C)) TogglePanel(_panelProfile);
        if (Input.GetKeyDown(KeyCode.K)) TogglePanel(_panelCampaigns);
        if (Input.GetKeyDown(KeyCode.M)) TogglePanel(_panelMap);
        if (Input.GetKeyDown(KeyCode.G)) TogglePanel(_panelGuild);
        if (Input.GetKeyDown(KeyCode.O)) TogglePanel(_panelSettings);
        if (Input.GetKeyDown(KeyCode.Escape)) ClosePanels();
    }

    // ---------------------------------------------------------------- infra

    private void EnsureEventSystem()
    {
        if (FindAnyObjectByType<EventSystem>() != null) return;
        var go = new GameObject("EventSystem");
        go.AddComponent<EventSystem>();
        go.AddComponent<StandaloneInputModule>();
    }

    private void EnsureCamera()
    {
        if (Camera.main != null) return;
        var go = new GameObject("Main Camera");
        var cam = go.AddComponent<Camera>();
        cam.clearFlags = CameraClearFlags.SolidColor;
        cam.backgroundColor = BG;
        go.tag = "MainCamera";
    }

    private void BuildCanvas()
    {
        var go = new GameObject("UICanvas");
        _canvas = go.AddComponent<Canvas>();
        _canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        _canvas.sortingOrder = 100;

        var scaler = go.AddComponent<CanvasScaler>();
        scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1920, 1080);
        scaler.matchWidthOrHeight = 0.5f;

        go.AddComponent<GraphicRaycaster>();
        _root = go.GetComponent<RectTransform>();

        var bg = MakePanel(_root, "Background", BG, Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
        bg.SetAsFirstSibling();
    }

    private RectTransform MakePanel(Transform parent, string name, Color color,
        Vector2 anchorMin, Vector2 anchorMax, Vector2 offsetMin, Vector2 offsetMax)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Image));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.anchorMin = anchorMin;
        rt.anchorMax = anchorMax;
        rt.offsetMin = offsetMin;
        rt.offsetMax = offsetMax;
        go.GetComponent<Image>().color = color;
        return rt;
    }

    private static RectTransform Rect(GameObject go)
    {
        return go.GetComponent<RectTransform>();
    }

    private Text MakeText(Transform parent, string name, string value, int size, Color color,
        TextAnchor anchor, Vector2 anchorMin, Vector2 anchorMax, Vector2 offsetMin, Vector2 offsetMax)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Text));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.anchorMin = anchorMin;
        rt.anchorMax = anchorMax;
        rt.offsetMin = offsetMin;
        rt.offsetMax = offsetMax;

        var t = go.GetComponent<Text>();
        t.text = value;
        t.font = _font;
        t.fontSize = size;
        t.color = color;
        t.alignment = anchor;
        t.horizontalOverflow = HorizontalWrapMode.Wrap;
        t.verticalOverflow = VerticalWrapMode.Overflow;
        t.supportRichText = true;
        return t;
    }

    private Button MakeButton(Transform parent, string name, string label, Vector2 size,
        Vector2 position, Color bg, Color textColor, System.Action onClick)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Image), typeof(Button));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.sizeDelta = size;
        rt.anchoredPosition = position;

        var img = go.GetComponent<Image>();
        img.color = bg;

        var btn = go.GetComponent<Button>();
        var colors = btn.colors;
        colors.normalColor = Color.white;
        colors.highlightedColor = new Color(1.15f, 1.15f, 1.15f, 1f);
        colors.pressedColor = new Color(0.85f, 0.85f, 0.85f, 1f);
        colors.selectedColor = Color.white;
        colors.disabledColor = new Color(1f, 1f, 1f, 0.4f);
        btn.colors = colors;
        btn.targetGraphic = img;

        MakeText(rt, "Label", label, 20, textColor, TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, new Vector2(6, 0), new Vector2(-6, 0));

        if (onClick != null)
            btn.onClick.AddListener(() => onClick());

        return btn;
    }

    private Image MakeBar(Transform parent, string name, Color fillColor, Vector2 size,
        Vector2 position, float fillPercent)
    {
        var bg = new GameObject(name, typeof(RectTransform), typeof(Image));
        var bgRt = bg.GetComponent<RectTransform>();
        bgRt.SetParent(parent, false);
        bgRt.sizeDelta = size;
        bgRt.anchoredPosition = position;
        bg.GetComponent<Image>().color = new Color(0.15f, 0.14f, 0.18f, 1f);

        var fill = new GameObject("Fill", typeof(RectTransform), typeof(Image));
        var fillRt = fill.GetComponent<RectTransform>();
        fillRt.SetParent(bgRt, false);
        fillRt.anchorMin = new Vector2(0, 0);
        fillRt.anchorMax = new Vector2(Mathf.Clamp01(fillPercent / 100f), 1f);
        fillRt.offsetMin = new Vector2(1, 1);
        fillRt.offsetMax = new Vector2(-1, -1);
        var fillImg = fill.GetComponent<Image>();
        fillImg.color = fillColor;

        return fillImg;
    }

    private InputField MakeInput(Transform parent, string name, string placeholder, string value,
        Vector2 size, Vector2 position)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Image), typeof(InputField));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.sizeDelta = size;
        rt.anchoredPosition = position;
        go.GetComponent<Image>().color = PANEL_ALT;

        var textGo = new GameObject("Text", typeof(RectTransform), typeof(Text));
        var textRt = textGo.GetComponent<RectTransform>();
        textRt.SetParent(rt, false);
        textRt.anchorMin = Vector2.zero;
        textRt.anchorMax = Vector2.one;
        textRt.offsetMin = new Vector2(10, 4);
        textRt.offsetMax = new Vector2(-10, -4);

        var text = textGo.GetComponent<Text>();
        text.font = _font;
        text.fontSize = 17;
        text.color = TEXT;
        text.alignment = TextAnchor.MiddleLeft;
        text.supportRichText = false;

        var phGo = new GameObject("Placeholder", typeof(RectTransform), typeof(Text));
        var phRt = phGo.GetComponent<RectTransform>();
        phRt.SetParent(rt, false);
        phRt.anchorMin = Vector2.zero;
        phRt.anchorMax = Vector2.one;
        phRt.offsetMin = new Vector2(10, 4);
        phRt.offsetMax = new Vector2(-10, -4);

        var ph = phGo.GetComponent<Text>();
        ph.text = placeholder;
        ph.font = _font;
        ph.fontSize = 17;
        ph.color = MUTED;
        ph.alignment = TextAnchor.MiddleLeft;

        var input = go.GetComponent<InputField>();
        input.targetGraphic = go.GetComponent<Image>();
        input.textComponent = text;
        input.placeholder = ph;
        input.lineType = InputField.LineType.SingleLine;
        input.text = value ?? string.Empty;

        return input;
    }

    private Dropdown MakeDropdown(Transform parent, string name, string[] options,
        Vector2 size, Vector2 position)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Image), typeof(Dropdown));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.sizeDelta = size;
        rt.anchoredPosition = position;
        go.GetComponent<Image>().color = PANEL_ALT;

        var labelGo = new GameObject("Label", typeof(RectTransform), typeof(Text));
        var labelRt = labelGo.GetComponent<RectTransform>();
        labelRt.SetParent(rt, false);
        labelRt.anchorMin = Vector2.zero;
        labelRt.anchorMax = Vector2.one;
        labelRt.offsetMin = new Vector2(10, 0);
        labelRt.offsetMax = new Vector2(-30, 0);

        var label = labelGo.GetComponent<Text>();
        label.font = _font;
        label.fontSize = 16;
        label.color = TEXT;
        label.alignment = TextAnchor.MiddleLeft;

        var caption = new GameObject("Caption", typeof(RectTransform), typeof(Text));
        var capRt = caption.GetComponent<RectTransform>();
        capRt.SetParent(rt, false);
        capRt.anchorMin = new Vector2(1, 0);
        capRt.anchorMax = new Vector2(1, 1);
        capRt.offsetMin = new Vector2(-26, 6);
        capRt.offsetMax = new Vector2(-8, -6);

        var capText = caption.GetComponent<Text>();
        capText.text = "▼";
        capText.font = _font;
        capText.fontSize = 14;
        capText.color = MUTED;
        capText.alignment = TextAnchor.MiddleCenter;

        var dd = go.GetComponent<Dropdown>();
        dd.targetGraphic = go.GetComponent<Image>();
        dd.captionText = label;
        dd.captionImage = null;
        dd.template = BuildDropdownTemplate(dd);

        var list = new List<Dropdown.OptionData>();
        foreach (var o in options) list.Add(new Dropdown.OptionData(o));
        dd.options = list;
        dd.value = 0;
        dd.RefreshShownValue();

        return dd;
    }

    private RectTransform BuildDropdownTemplate(Dropdown dd)
    {
        var template = new GameObject("Template", typeof(RectTransform), typeof(Image), typeof(ScrollRect));
        var templateRt = template.GetComponent<RectTransform>();
        templateRt.SetParent(dd.transform, false);
        templateRt.anchorMin = new Vector2(0, 0);
        templateRt.anchorMax = new Vector2(1, 0);
        templateRt.pivot = new Vector2(0.5f, 1f);
        templateRt.anchoredPosition = new Vector2(0, 2);
        templateRt.sizeDelta = new Vector2(0, 150);
        template.SetActive(false);

        template.GetComponent<Image>().color = PANEL_ALT;

        var viewport = new GameObject("Viewport", typeof(RectTransform), typeof(Image), typeof(Mask));
        var vpRt = viewport.GetComponent<RectTransform>();
        vpRt.SetParent(templateRt, false);
        vpRt.anchorMin = Vector2.zero;
        vpRt.anchorMax = Vector2.one;
        vpRt.offsetMin = Vector2.zero;
        vpRt.offsetMax = Vector2.zero;
        viewport.GetComponent<Image>().color = new Color(0, 0, 0, 0.01f);
        viewport.GetComponent<Mask>().showMaskGraphic = false;

        var content = new GameObject("Content", typeof(RectTransform));
        var contentRt = content.GetComponent<RectTransform>();
        contentRt.SetParent(vpRt, false);
        contentRt.anchorMin = new Vector2(0, 1);
        contentRt.anchorMax = new Vector2(1, 1);
        contentRt.pivot = new Vector2(0.5f, 1f);
        contentRt.sizeDelta = new Vector2(0, 30);

        var item = new GameObject("Item", typeof(RectTransform), typeof(Toggle));
        var itemRt = item.GetComponent<RectTransform>();
        itemRt.SetParent(contentRt, false);
        itemRt.anchorMin = new Vector2(0, 0.5f);
        itemRt.anchorMax = new Vector2(1, 0.5f);
        itemRt.sizeDelta = new Vector2(0, 30);

        var itemBg = new GameObject("Item Background", typeof(RectTransform), typeof(Image));
        var ibRt = itemBg.GetComponent<RectTransform>();
        ibRt.SetParent(itemRt, false);
        ibRt.anchorMin = Vector2.zero;
        ibRt.anchorMax = Vector2.one;
        ibRt.offsetMin = Vector2.zero;
        ibRt.offsetMax = Vector2.zero;
        var ibImg = itemBg.GetComponent<Image>();
        ibImg.color = new Color(1, 1, 1, 0.04f);

        var itemLabelGo = new GameObject("Item Label", typeof(RectTransform), typeof(Text));
        var ilRt = itemLabelGo.GetComponent<RectTransform>();
        ilRt.SetParent(itemRt, false);
        ilRt.anchorMin = Vector2.zero;
        ilRt.anchorMax = Vector2.one;
        ilRt.offsetMin = new Vector2(10, 0);
        ilRt.offsetMax = new Vector2(-10, 0);

        var itemLabel = itemLabelGo.GetComponent<Text>();
        itemLabel.font = _font;
        itemLabel.fontSize = 16;
        itemLabel.color = TEXT;
        itemLabel.alignment = TextAnchor.MiddleLeft;

        var toggle = item.GetComponent<Toggle>();
        toggle.targetGraphic = ibImg;
        toggle.graphic = null;
        toggle.isOn = true;

        dd.template = templateRt;
        dd.captionText = dd.captionText;

        var scroll = template.GetComponent<ScrollRect>();
        scroll.content = contentRt;
        scroll.viewport = vpRt;
        scroll.horizontal = false;
        scroll.movementType = ScrollRect.MovementType.Clamped;

        return templateRt;
    }

    // ---------------------------------------------------------------- main menu

    private void BuildMainMenu()
    {
        var root = MakePanel(_root, "MainMenu", BG, Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
        _mainMenu = root.gameObject;

        MakePanel(root, "Glow", new Color(0.55f, 0.09f, 0.17f, 0.35f),
            new Vector2(0f, 0.58f), new Vector2(1f, 1f), Vector2.zero, Vector2.zero);

        MakeText(root, "Title", "ЧЁРНАЯ МАТЕРИЯ", 96, GOLD, TextAnchor.MiddleCenter,
            new Vector2(0, 0.66f), new Vector2(1, 0.82f), Vector2.zero, Vector2.zero);

        MakeText(root, "Subtitle", "BLACK MATTER   ·   MMORPG", 24, MUTED, TextAnchor.MiddleCenter,
            new Vector2(0, 0.60f), new Vector2(1, 0.66f), Vector2.zero, Vector2.zero);

        MakeText(root, "Tagline", "Три силы. Один раскол. Мир на грани Заражения.", 19, MUTED,
            TextAnchor.MiddleCenter, new Vector2(0, 0.54f), new Vector2(1, 0.59f), Vector2.zero, Vector2.zero);

        var buttons = new GameObject("MenuButtons", typeof(RectTransform));
        var bRt = buttons.GetComponent<RectTransform>();
        bRt.SetParent(root, false);
        bRt.anchorMin = new Vector2(0.5f, 0.5f);
        bRt.anchorMax = new Vector2(0.5f, 0.5f);
        bRt.sizeDelta = new Vector2(360, 260);
        bRt.anchoredPosition = new Vector2(0, -110);

        MakeButton(bRt, "PlayButton", "Играть", new Vector2(360, 56), new Vector2(0, 84), ACCENT, TEXT, ShowCharacterSelect);
        MakeButton(bRt, "WorldButton", "О мире", new Vector2(360, 52), new Vector2(0, 22), PANEL_ALT, TEXT,
            () => ShowToast("Орден Света · Тёмный Орден · Нейтральные. Кампании 70–99."));
        MakeButton(bRt, "SettingsButton", "Настройки", new Vector2(360, 52), new Vector2(0, -36), PANEL_ALT, TEXT,
            () => { ShowHud(); TogglePanel(_panelSettings); });
        MakeButton(bRt, "QuitButton", "Выход", new Vector2(360, 52), new Vector2(0, -94), new Color(0, 0, 0, 0), MUTED, QuitGame);

        MakeText(root, "Footer", "v0.2 · клиент на uGUI · Unity 7", 14, new Color(0.47f, 0.45f, 0.51f),
            TextAnchor.MiddleCenter, new Vector2(0, 0.02f), new Vector2(1, 0.07f), Vector2.zero, Vector2.zero);
    }

    private void QuitGame()
    {
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
    }

    // ---------------------------------------------------------------- char select

    private void BuildCharacterSelect()
    {
        var root = MakePanel(_root, "CharSelect", BG, new Vector2(0.12f, 0.10f), new Vector2(0.88f, 0.90f),
            Vector2.zero, Vector2.zero);
        _charSelect = root.gameObject;

        var header = MakePanel(root, "Header", PANEL,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(0, -64), Vector2.zero);

        MakeText(header, "Title", "Выбор персонажа", 26, GOLD, TextAnchor.MiddleLeft,
            Vector2.zero, Vector2.one, new Vector2(24, 0), new Vector2(-90, 0));

        MakeButton(header, "Back", "×", new Vector2(40, 40), new Vector2(-28, 0),
            new Color(0.72f, 0.22f, 0.28f, 0.25f), TEXT, ShowMainMenu);

        // list
        var listPanel = MakePanel(root, "List", PANEL_ALT,
            new Vector2(0, 0), new Vector2(0.58f, 1), new Vector2(20, 20), new Vector2(-10, -80));

        var cards = new[]
        {
            ("Странник", "70 · Жрец · Орден Света"),
            ("Таллон", "48 · Маг · Тёмный Орден"),
            ("Вилл", "12 · Плут · Нейтральные"),
        };

        float y = -20f;
        foreach (var c in cards)
        {
            var row = MakePanel(listPanel, "Card", new Color(0.16f, 0.14f, 0.19f, 1f),
                new Vector2(0, 1), new Vector2(1, 1), new Vector2(12, y - 74), new Vector2(-12, y));

            MakeText(row, "Name", c.Item1, 22, TEXT, TextAnchor.MiddleLeft,
                new Vector2(0, 0.5f), new Vector2(1, 1), new Vector2(18, 0), new Vector2(-18, 0));
            MakeText(row, "Meta", c.Item2, 16, MUTED, TextAnchor.MiddleLeft,
                new Vector2(0, 0), new Vector2(1, 0.5f), new Vector2(18, 0), new Vector2(-18, 0));

            string charName = c.Item1;
            var btn = row.gameObject.AddComponent<Button>();
            btn.targetGraphic = row.GetComponent<Image>();
            btn.onClick.AddListener(() => SelectCharacter(charName));

            y -= 86f;
        }

        // create form
        var form = MakePanel(root, "CreateForm", PANEL_ALT,
            new Vector2(0.58f, 0), new Vector2(1, 1), new Vector2(10, 20), new Vector2(-20, -80));

        MakeText(form, "Title", "Новый персонаж", 19, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, -46), new Vector2(-20, -12));

        MakeText(form, "NameLabel", "Имя", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, -78), new Vector2(-20, -56));
        _charName = MakeInput(form, "NameInput", "Странник", "",
            new Vector2(-40, 38), new Vector2(0, -100));

        MakeText(form, "RaceLabel", "Раса", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, -148), new Vector2(-20, -126));
        _charRace = MakeDropdown(form, "RaceDropdown",
            new[] { "Человек", "Эльф", "Дварф", "Орк", "Полурослик" },
            new Vector2(-40, 38), new Vector2(0, -170));

        MakeText(form, "ClassLabel", "Класс", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, -218), new Vector2(-20, -196));
        _charClass = MakeDropdown(form, "ClassDropdown",
            new[] { "Воин", "Маг", "Жрец", "Плут", "Охотник", "Колдун" },
            new Vector2(-40, 38), new Vector2(0, -240));
        _charClass.value = 2;

        MakeText(form, "FactionLabel", "Фракция", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, -288), new Vector2(-20, -266));
        _charFaction = MakeDropdown(form, "FactionDropdown",
            new[] { "Орден Света", "Тёмный Орден", "Нейтральные" },
            new Vector2(-40, 38), new Vector2(0, -310));

        MakeButton(form, "Create", "Создать и войти", new Vector2(-40, 46), new Vector2(0, -380),
            ACCENT, TEXT, CreateCharacter);
    }

    private void SelectCharacter(string charName)
    {
        if (_hudName != null) _hudName.text = charName;
        if (_profileName != null) _profileName.text = charName;
        ShowHud();
        ShowToast($"Вход за «{charName}»");
    }

    private void CreateCharacter()
    {
        string charName = _charName != null && !string.IsNullOrWhiteSpace(_charName.text)
            ? _charName.text : "Странник";

        if (_hudName != null) _hudName.text = charName;
        if (_profileName != null) _profileName.text = charName;

        if (_profileClass != null && _charClass != null)
            _profileClass.text = $"Уровень 1 · {_charClass.options[_charClass.value].text}";

        if (_profileRace != null && _charRace != null && _charFaction != null)
            _profileRace.text = $"{_charRace.options[_charRace.value].text} · {_charFaction.options[_charFaction.value].text}";

        if (_hudFaction != null && _charFaction != null)
            _hudFaction.text = _charFaction.options[_charFaction.value].text;

        ShowHud();
        ShowToast($"Персонаж «{charName}» создан");
    }

    // ---------------------------------------------------------------- HUD

    private void BuildHud()
    {
        var root = MakePanel(_root, "HUD", new Color(0, 0, 0, 0),
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
        _hud = root.gameObject;

        BuildTopBar(root);
        BuildVitals(root);
        BuildChat(root);
        BuildHotkeys(root);
        BuildQuestPanel(root);
        BuildInventoryPanel(root);
        BuildProfilePanel(root);
        BuildCampaignsPanel(root);
        BuildMapPanel(root);
        BuildGuildPanel(root);
        BuildSettingsPanel(root);
    }

    private void BuildTopBar(Transform root)
    {
        var bar = MakePanel(root, "TopBar", new Color(0.06f, 0.05f, 0.08f, 0.94f),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(0, -56), Vector2.zero);

        _hudName = MakeText(bar, "Name", "Странник", 20, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 0), new Vector2(0.22f, 1), new Vector2(22, 0), Vector2.zero);

        _hudLevel = MakeText(bar, "Level", "70 ур.", 15, TEXT, TextAnchor.MiddleLeft,
            new Vector2(0.22f, 0), new Vector2(0.32f, 1), Vector2.zero, Vector2.zero);

        _hudFaction = MakeText(bar, "Faction", "Орден Света", 15, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0.32f, 0), new Vector2(0.50f, 1), Vector2.zero, Vector2.zero);

        _hudZone = MakeText(bar, "Zone", "Ательгард", 17, MUTED, TextAnchor.MiddleCenter,
            new Vector2(0.50f, 0), new Vector2(0.66f, 1), Vector2.zero, Vector2.zero);

        _hudClock = MakeText(bar, "Clock", "12:00", 15, MUTED, TextAnchor.MiddleRight,
            new Vector2(0.66f, 0), new Vector2(0.74f, 1), Vector2.zero, Vector2.zero);

        _hudWeather = MakeText(bar, "Weather", "Ясно", 15, MUTED, TextAnchor.MiddleRight,
            new Vector2(0.74f, 0), new Vector2(0.83f, 1), Vector2.zero, Vector2.zero);

        _hudGold = MakeText(bar, "Gold", "1 250", 17, GOLD, TextAnchor.MiddleRight,
            new Vector2(0.83f, 0), new Vector2(0.92f, 1), Vector2.zero, Vector2.zero);

        _hudSpirit = MakeText(bar, "Spirit", "40", 17, new Color(0.59f, 0.78f, 1f), TextAnchor.MiddleRight,
            new Vector2(0.92f, 0), new Vector2(1f, 1), Vector2.zero, new Vector2(-22, 0));
    }

    private void BuildVitals(Transform root)
    {
        var box = MakePanel(root, "Vitals", new Color(0.06f, 0.05f, 0.08f, 0.9f),
            new Vector2(0, 0), new Vector2(0, 0), new Vector2(20, 20), new Vector2(340, 172));

        _hpLabel = MakeText(box, "HpLabel", "HP 820 / 1000", 14, TEXT, TextAnchor.UpperLeft,
            new Vector2(0, 0.86f), new Vector2(1, 1), new Vector2(16, 0), new Vector2(-16, 0));
        MakeBar(box, "HpBar", HP, new Vector2(-32, 18), new Vector2(0, -44), 82f);

        _mpLabel = MakeText(box, "MpLabel", "MP 640 / 1000", 14, TEXT, TextAnchor.UpperLeft,
            new Vector2(0, 0.58f), new Vector2(1, 0.72f), new Vector2(16, 0), new Vector2(-16, 0));
        MakeBar(box, "MpBar", MP, new Vector2(-32, 18), new Vector2(0, -76), 64f);

        _xpLabel = MakeText(box, "XpLabel", "XP 4 200 / 9 000", 13, MUTED, TextAnchor.UpperLeft,
            new Vector2(0, 0.30f), new Vector2(1, 0.44f), new Vector2(16, 0), new Vector2(-16, 0));
        MakeBar(box, "XpBar", XP, new Vector2(-32, 10), new Vector2(0, -104), 46f);

        MakeText(box, "Zone", "Ательгард · Орден Света", 13, MUTED, TextAnchor.MiddleCenter,
            Vector2.zero, new Vector2(1, 0.22f), Vector2.zero, Vector2.zero);

        _hpFill = (RectTransform)box.Find("HpBar/Fill");
        _mpFill = (RectTransform)box.Find("MpBar/Fill");
        _xpFill = (RectTransform)box.Find("XpBar/Fill");
    }

    private void BuildChat(Transform root)
    {
        var box = MakePanel(root, "Chat", new Color(0.06f, 0.05f, 0.08f, 0.86f),
            new Vector2(0, 0), new Vector2(0, 0), new Vector2(20, 210), new Vector2(540, 520));

        var header = MakePanel(box, "Header", new Color(0.11f, 0.10f, 0.14f, 1f),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(0, -36), Vector2.zero);

        _chatChannel = MakeText(header, "Channel", "Общий", 15, GOLD, TextAnchor.MiddleLeft,
            Vector2.zero, new Vector2(0.6f, 1), new Vector2(12, 0), Vector2.zero);

        MakeButton(header, "ChannelButton", "канал", new Vector2(80, 26), new Vector2(-50, 0),
            new Color(0.22f, 0.19f, 0.26f, 0.8f), MUTED, CycleChannel);

        MakeButton(header, "Close", "×", new Vector2(26, 26), new Vector2(-14, 0),
            new Color(0, 0, 0, 0), MUTED, HideChat);

        // scroll
        var scrollGo = new GameObject("ChatScroll", typeof(RectTransform), typeof(Image), typeof(ScrollRect));
        var scrollRt = scrollGo.GetComponent<RectTransform>();
        scrollRt.SetParent(box, false);
        scrollRt.anchorMin = Vector2.zero;
        scrollRt.anchorMax = Vector2.one;
        scrollRt.offsetMin = new Vector2(8, 52);
        scrollRt.offsetMax = new Vector2(-8, -40);
        scrollGo.GetComponent<Image>().color = new Color(0, 0, 0, 0.01f);

        var viewport = MakePanel(scrollRt, "Viewport", new Color(0, 0, 0, 0),
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
        viewport.gameObject.AddComponent<Mask>().showMaskGraphic = false;

        var content = MakePanel(viewport, "Content", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), Vector2.zero, new Vector2(0, 0));
        content.pivot = new Vector2(0.5f, 1f);
        _chatContent = content;

        var scroll = scrollGo.GetComponent<ScrollRect>();
        scroll.content = content;
        scroll.viewport = viewport;
        scroll.horizontal = false;
        scroll.movementType = ScrollRect.MovementType.Clamped;
        scroll.scrollSensitivity = 30f;
        _chatScroll = scroll;

        var layout = content.gameObject.AddComponent<VerticalLayoutGroup>();
        layout.childForceExpandHeight = false;
        layout.childControlHeight = true;
        layout.childControlWidth = true;
        layout.spacing = 2;
        layout.padding = new RectOffset(6, 6, 6, 6);

        var fitter = content.gameObject.AddComponent<ContentSizeFitter>();
        fitter.verticalFit = ContentSizeFitter.FitMode.PreferredSize;

        // input row
        var row = MakePanel(box, "InputRow", new Color(0.11f, 0.10f, 0.14f, 1f),
            new Vector2(0, 0), new Vector2(1, 0), new Vector2(0, 0), new Vector2(0, 44));

        _chatInput = MakeInput(row, "ChatInput", "Написать сообщение…", "",
            new Vector2(-70, 32), new Vector2(-30, 0));
        _chatInput.characterLimit = 256;
        _chatInput.onEndEdit.AddListener(OnChatSubmit);

        MakeButton(row, "Send", "➤", new Vector2(44, 32), new Vector2(6, 0), ACCENT, TEXT, SendChatInput);
    }

    private void HideChat()
    {
        var chat = _hud.transform.Find("Chat");
        if (chat != null) chat.gameObject.SetActive(false);
    }

    private void CycleChannel()
    {
        int i = System.Array.IndexOf(Channels, _channel);
        _channel = Channels[(i + 1) % Channels.Length];
        if (_chatChannel != null) _chatChannel.text = _channel;
    }

    private void OnChatSubmit(string value)
    {
        if (!string.IsNullOrWhiteSpace(value))
            SendChatInput();
    }

    private void SendChatInput()
    {
        if (_chatInput == null) return;

        string text = _chatInput.text;
        if (string.IsNullOrWhiteSpace(text)) return;

        _chatInput.text = string.Empty;
        AddChatLine("Странник", text);

        if (GameManager.Instance != null && GameManager.Instance.ChatManager != null)
            GameManager.Instance.ChatManager.SendMessage(1, text, _channel, 0);

        _chatInput.ActivateInputField();
    }

    private void AddChatLine(string author, string message)
    {
        if (_chatContent == null) return;

        var go = new GameObject("Line", typeof(RectTransform), typeof(Text));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(_chatContent, false);

        var t = go.GetComponent<Text>();
        t.font = _font;
        t.fontSize = 15;
        t.color = TEXT;
        t.alignment = TextAnchor.UpperLeft;
        t.horizontalOverflow = HorizontalWrapMode.Wrap;
        t.verticalOverflow = VerticalWrapMode.Overflow;
        t.supportRichText = true;
        t.text = $"<color=#EFCC8C>[{author}]</color> {message}";

        var le = go.AddComponent<LayoutElement>();
        le.minHeight = 20;

        if (_chatScroll != null)
            _chatScroll.verticalNormalizedPosition = 0f;
    }

    private void BuildHotkeys(Transform root)
    {
        var bar = new GameObject("Hotkeys", typeof(RectTransform));
        var rt = bar.GetComponent<RectTransform>();
        rt.SetParent(root, false);
        rt.anchorMin = new Vector2(0.5f, 0);
        rt.anchorMax = new Vector2(0.5f, 0);
        rt.sizeDelta = new Vector2(880, 44);
        rt.anchoredPosition = new Vector2(0, 26);

        var keys = new (string label, System.Action action)[]
        {
            ("L · Квесты", () => TogglePanel(_panelQuests)),
            ("I · Сумка", () => TogglePanel(_panelInventory)),
            ("C · Профиль", () => TogglePanel(_panelProfile)),
            ("K · Кампании", () => TogglePanel(_panelCampaigns)),
            ("M · Карта", () => TogglePanel(_panelMap)),
            ("G · Гильдия", () => TogglePanel(_panelGuild)),
            ("O · Настройки", () => TogglePanel(_panelSettings)),
            ("Меню", ShowMainMenu),
        };

        float x = -385f;
        foreach (var k in keys)
        {
            MakeButton(rt, "Hotkey", k.label, new Vector2(94, 40), new Vector2(x, 0),
                new Color(0.10f, 0.09f, 0.125f, 0.94f), TEXT, k.action);
            x += 100f;
        }
    }

    // ---------------------------------------------------------------- panels

    private GameObject MakeWindow(Transform root, string name, string title, Vector2 size, out RectTransform body)
    {
        var win = MakePanel(root, name, PANEL,
            new Vector2(0.5f, 0.5f), new Vector2(0.5f, 0.5f),
            new Vector2(-size.x / 2, -size.y / 2), new Vector2(size.x / 2, size.y / 2));

        var header = MakePanel(win, "Header", new Color(0.12f, 0.105f, 0.145f, 1f),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(0, -54), Vector2.zero);

        MakeText(header, "Title", title, 24, GOLD, TextAnchor.MiddleLeft,
            Vector2.zero, Vector2.one, new Vector2(20, 0), new Vector2(-60, 0));

        MakeButton(header, "Close", "×", new Vector2(34, 34), new Vector2(-24, 0),
            new Color(0.72f, 0.22f, 0.28f, 0.25f), TEXT, () => win.gameObject.SetActive(false));

        body = MakePanel(win, "Body", new Color(0, 0, 0, 0),
            Vector2.zero, Vector2.one, new Vector2(0, 0), new Vector2(0, -54));

        win.gameObject.SetActive(false);
        return win.gameObject;
    }

    private void BuildQuestPanel(Transform root)
    {
        RectTransform body;
        _panelQuests = MakeWindow(root, "PanelQuests", "Журнал квестов", new Vector2(640, 560), out body);

        _questList = MakePanel(body, "List", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(0, 0), new Vector2(0, 0));
        _questList.pivot = new Vector2(0.5f, 1f);

        var vlg = _questList.gameObject.AddComponent<VerticalLayoutGroup>();
        vlg.padding = new RectOffset(16, 16, 14, 14);
        vlg.spacing = 10;
        vlg.childControlHeight = true;
        vlg.childControlWidth = true;
        vlg.childForceExpandHeight = false;

        var fitter = _questList.gameObject.AddComponent<ContentSizeFitter>();
        fitter.verticalFit = ContentSizeFitter.FitMode.PreferredSize;
    }

    private void BuildInventoryPanel(Transform root)
    {
        RectTransform body;
        _panelInventory = MakeWindow(root, "PanelInventory", "Сумка", new Vector2(780, 620), out body);

        _inventoryCount = MakeText(body, "Count", "12 / 24 слотов", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(0.5f, 1), new Vector2(18, -30), Vector2.zero);

        _inventoryGold = MakeText(body, "Gold", "Золото: 1 250", 16, GOLD, TextAnchor.MiddleRight,
            new Vector2(0.5f, 1), new Vector2(0.8f, 1), Vector2.zero, new Vector2(0, -30));

        _inventorySpirit = MakeText(body, "Spirit", "Дух: 40", 16, new Color(0.59f, 0.78f, 1f), TextAnchor.MiddleRight,
            new Vector2(0.8f, 1), new Vector2(1, 1), Vector2.zero, new Vector2(-18, -30));

        _inventoryGrid = MakePanel(body, "Grid", new Color(0, 0, 0, 0),
            Vector2.zero, Vector2.one, new Vector2(16, 16), new Vector2(-16, -48));

        var grid = _inventoryGrid.gameObject.AddComponent<GridLayoutGroup>();
        grid.cellSize = new Vector2(84, 84);
        grid.spacing = new Vector2(8, 8);
        grid.padding = new RectOffset(4, 4, 4, 4);
    }

    private void BuildProfilePanel(Transform root)
    {
        RectTransform body;
        _panelProfile = MakeWindow(root, "PanelProfile", "Профиль", new Vector2(620, 620), out body);

        var top = MakePanel(body, "Top", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -120), new Vector2(-18, -12));

        var avatar = MakePanel(top, "Avatar", new Color(0.48f, 0.14f, 0.19f, 1f),
            new Vector2(0, 0), new Vector2(0, 1), new Vector2(0, 0), new Vector2(96, 0));
        MakeText(avatar, "Glyph", "◈", 40, GOLD, TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);

        _profileName = MakeText(top, "Name", "Странник", 24, TEXT, TextAnchor.UpperLeft,
            new Vector2(0, 0.56f), new Vector2(1, 1), new Vector2(112, 0), Vector2.zero);
        _profileClass = MakeText(top, "Class", "Уровень 70 · Жрец Света", 15, MUTED, TextAnchor.UpperLeft,
            new Vector2(0, 0.32f), new Vector2(1, 0.56f), new Vector2(112, 0), Vector2.zero);
        _profileRace = MakeText(top, "Race", "Человек · Орден Света", 15, MUTED, TextAnchor.UpperLeft,
            new Vector2(0, 0.08f), new Vector2(1, 0.32f), new Vector2(112, 0), Vector2.zero);

        MakeText(body, "StatsTitle", "Характеристики", 17, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -156), new Vector2(-18, -126));

        _statsList = MakePanel(body, "Stats", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -480), new Vector2(-18, -160));

        var vlg = _statsList.gameObject.AddComponent<VerticalLayoutGroup>();
        vlg.spacing = 8;
        vlg.childControlHeight = false;
        vlg.childControlWidth = true;
        vlg.childForceExpandHeight = false;
    }

    private void BuildCampaignsPanel(Transform root)
    {
        RectTransform body;
        _panelCampaigns = MakeWindow(root, "PanelCampaigns", "Кампании 70+", new Vector2(820, 560), out body);

        _campaignList = MakePanel(body, "List", new Color(0, 0, 0, 0),
            Vector2.zero, Vector2.one, new Vector2(16, 16), new Vector2(-16, -16));

        var vlg = _campaignList.gameObject.AddComponent<VerticalLayoutGroup>();
        vlg.spacing = 10;
        vlg.childControlHeight = false;
        vlg.childControlWidth = true;
        vlg.childForceExpandHeight = false;
    }

    private void BuildMapPanel(Transform root)
    {
        RectTransform body;
        _panelMap = MakeWindow(root, "PanelMap", "Карта мира", new Vector2(880, 600), out body);

        var canvas = MakePanel(body, "MapCanvas", new Color(0.07f, 0.06f, 0.10f, 1f),
            Vector2.zero, Vector2.one, new Vector2(16, 56), new Vector2(-16, -16));

        var light = MakePanel(canvas, "Light", new Color(0.937f, 0.80f, 0.55f, 0.16f),
            new Vector2(0, 1), new Vector2(0, 1), new Vector2(24, -190), new Vector2(300, -24));
        MakeText(light, "Label", "Ательгард", 17, TEXT, TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);

        var dark = MakePanel(canvas, "Dark", new Color(0.48f, 0.14f, 0.19f, 0.30f),
            new Vector2(1, 1), new Vector2(1, 1), new Vector2(-280, -210), new Vector2(-24, -24));
        MakeText(dark, "Label", "Умбра", 17, TEXT, TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);

        var neutral = MakePanel(canvas, "Neutral", new Color(0.47f, 0.59f, 0.47f, 0.16f),
            new Vector2(0.5f, 0), new Vector2(0.5f, 0), new Vector2(-190, 24), new Vector2(190, 170));
        MakeText(neutral, "Label", "Грейвхейвен", 17, TEXT, TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);

        MakePanel(canvas, "PlayerMarker", GOLD,
            new Vector2(0, 1), new Vector2(0, 1), new Vector2(150, -110), new Vector2(166, -94));

        var legend = MakePanel(body, "Legend", new Color(0, 0, 0, 0),
            new Vector2(0, 0), new Vector2(1, 0), new Vector2(18, 16), new Vector2(-18, 48));

        MakeText(legend, "L1", "◆ Орден Света", 14, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 0), new Vector2(0.25f, 1), Vector2.zero, Vector2.zero);
        MakeText(legend, "L2", "◆ Тёмный Орден", 14, new Color(0.85f, 0.38f, 0.44f), TextAnchor.MiddleLeft,
            new Vector2(0.25f, 0), new Vector2(0.5f, 1), Vector2.zero, Vector2.zero);
        MakeText(legend, "L3", "◆ Нейтральные", 14, new Color(0.59f, 0.78f, 0.59f), TextAnchor.MiddleLeft,
            new Vector2(0.5f, 0), new Vector2(0.75f, 1), Vector2.zero, Vector2.zero);
        MakeText(legend, "L4", "● Вы здесь", 14, TEXT, TextAnchor.MiddleLeft,
            new Vector2(0.75f, 0), new Vector2(1, 1), Vector2.zero, Vector2.zero);
    }

    private void BuildGuildPanel(Transform root)
    {
        RectTransform body;
        _panelGuild = MakeWindow(root, "PanelGuild", "Гильдия", new Vector2(640, 600), out body);

        MakeText(body, "Name", "Клинок Рассвета", 22, TEXT, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -46), Vector2.zero);

        MakeText(body, "Meta", "Уровень гильдии: 4 · Участников: 12 / 40", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -74), Vector2.zero);

        MakeText(body, "WarTitle", "Активные войны", 17, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -114), Vector2.zero);

        _guildWarList = MakePanel(body, "Wars", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -260), new Vector2(-18, -122));
        AddVerticalLayout(_guildWarList.gameObject, 8);

        MakeText(body, "AllyTitle", "Союзники", 17, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -300), Vector2.zero);

        _guildAllyList = MakePanel(body, "Allies", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -470), new Vector2(-18, -308));
        AddVerticalLayout(_guildAllyList.gameObject, 8);
    }

    private void BuildSettingsPanel(Transform root)
    {
        RectTransform body;
        _panelSettings = MakeWindow(root, "PanelSettings", "Настройки", new Vector2(620, 620), out body);

        MakeText(body, "GfxTitle", "Графика", 17, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -44), Vector2.zero);

        MakeText(body, "QualityLabel", "Качество", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -76), Vector2.zero);
        MakeBar(body, "QualityBar", ACCENT, new Vector2(-36, 16), new Vector2(0, -96), 66f);

        MakeText(body, "UiTitle", "Интерфейс", 17, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -150), Vector2.zero);

        MakeText(body, "ScaleLabel", "Масштаб UI", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -182), Vector2.zero);
        MakeBar(body, "ScaleBar", ACCENT, new Vector2(-36, 16), new Vector2(0, -202), 50f);

        MakeText(body, "LangLabel", "Язык: Русский", 15, TEXT, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -244), Vector2.zero);

        MakeText(body, "NetTitle", "Сеть", 17, GOLD, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -290), Vector2.zero);

        MakeText(body, "ServerLabel", "Сервер", 15, MUTED, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(18, -322), Vector2.zero);
        MakeInput(body, "ServerInput", "127.0.0.1:8093", "127.0.0.1:8093",
            new Vector2(-36, 38), new Vector2(0, -344));

        MakeButton(body, "Apply", "Применить", new Vector2(-36, 44), new Vector2(0, -404),
            ACCENT, TEXT, () => ShowToast("Настройки сохранены"));
    }

    private static void AddVerticalLayout(GameObject go, int spacing)
    {
        var vlg = go.AddComponent<VerticalLayoutGroup>();
        vlg.spacing = spacing;
        vlg.childControlHeight = false;
        vlg.childControlWidth = true;
        vlg.childForceExpandHeight = false;
        vlg.childForceExpandWidth = true;
    }

    // ---------------------------------------------------------------- content

    private void BuildStaticContent()
    {
        AddChatLine("Система", "Добро пожаловать в Чёрную Материю.");
        AddChatLine("Система", "L квесты · I сумка · C профиль · K кампании · M карта · G гильдия.");
        AddChatLine("Виксис", "Свет не прощает отступников, но принимает кающихся.");

        BuildQuestRows();
        BuildInventoryGrid();
        BuildStatRows();
        BuildCampaignRows();
        BuildGuildRows();
    }

    private void BuildQuestRows()
    {
        if (_questList == null) return;

        var quests = new[]
        {
            ("Очищение окраин", "Очистите заражённые земли Ательгарда от скверны.", "3 / 8 зон"),
            ("Свет над руинами", "Восстановите солнечный храм на востоке.", "1 / 3 реликвии"),
            ("Голос Умбры", "Выслушайте Великого Мага Таллона.", "Готово к сдаче"),
        };

        foreach (var q in quests)
        {
            var row = MakePanel(_questList, "QuestRow", new Color(0.13f, 0.115f, 0.155f, 1f),
                Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
            var le = row.gameObject.AddComponent<LayoutElement>();
            le.minHeight = 74;

            var accent = MakePanel(row, "Accent", ACCENT,
                new Vector2(0, 0), new Vector2(0, 1), Vector2.zero, new Vector2(3, 0));

            MakeText(row, "Name", q.Item1, 18, TEXT, TextAnchor.UpperLeft,
                new Vector2(0, 0.62f), new Vector2(1, 1), new Vector2(16, 0), new Vector2(-14, -8));
            MakeText(row, "Desc", q.Item2, 14, MUTED, TextAnchor.UpperLeft,
                new Vector2(0, 0.30f), new Vector2(1, 0.62f), new Vector2(16, 0), new Vector2(-14, 0));
            MakeText(row, "Progress", q.Item3, 14, GOLD, TextAnchor.LowerLeft,
                new Vector2(0, 0), new Vector2(1, 0.30f), new Vector2(16, 6), new Vector2(-14, 0));
        }
    }

    private static readonly (string name, int count, string slot)[] Items =
    {
        ("Клинок Света", 1, "Оружие"),
        ("Щит Рассвета", 1, "Щит"),
        ("Зелье лечения", 5, "Расходник"),
        ("Свиток возврата", 2, "Расходник"),
        ("Кольцо силы", 1, "Аксессуар"),
        ("Руда мифрила", 12, "Ресурс"),
        ("Кожа волка", 8, "Ресурс"),
        ("Ключ подземелья", 1, "Ключ"),
        ("Осколок духа", 3, "Дух"),
        ("Карта Ательгарда", 1, "Документ"),
        ("Эликсир стойкости", 4, "Расходник"),
        ("Амулет духа", 1, "Аксессуар"),
    };

    private void BuildInventoryGrid()
    {
        if (_inventoryGrid == null) return;

        const int slots = 24;
        for (int i = 0; i < slots; i++)
        {
            var slot = MakePanel(_inventoryGrid, "Slot", new Color(0.13f, 0.115f, 0.155f, 1f),
                new Vector2(0, 1), new Vector2(0, 1), Vector2.zero, Vector2.zero);

            if (i < Items.Length)
            {
                MakeText(slot, "Name", Items[i].name, 11, MUTED, TextAnchor.LowerCenter,
                    Vector2.zero, Vector2.one, new Vector2(4, 4), new Vector2(-4, -20));

                if (Items[i].count > 1)
                    MakeText(slot, "Count", "x" + Items[i].count, 13, GOLD, TextAnchor.UpperRight,
                        Vector2.zero, Vector2.one, new Vector2(4, 4), new Vector2(-6, -4));

                string itemName = Items[i].name;
                string slotName = Items[i].slot;
                var btn = slot.gameObject.AddComponent<Button>();
                btn.targetGraphic = slot.GetComponent<Image>();
                btn.onClick.AddListener(() => ShowToast($"{itemName} · {slotName}"));
            }
            else
            {
                slot.GetComponent<Image>().color = new Color(0.10f, 0.09f, 0.12f, 0.6f);
            }
        }

        if (_inventoryCount != null) _inventoryCount.text = $"{Items.Length} / {slots} слотов";
    }

    private void BuildStatRows()
    {
        if (_statsList == null) return;

        var stats = new[]
        {
            ("Сила", 45f), ("Ловкость", 60f), ("Интеллект", 88f),
            ("Выносливость", 70f), ("Духовность", 95f),
        };

        foreach (var s in stats)
        {
            var row = MakePanel(_statsList, "StatRow", new Color(0, 0, 0, 0),
                new Vector2(0, 1), new Vector2(1, 1), Vector2.zero, Vector2.zero);
            var le = row.gameObject.AddComponent<LayoutElement>();
            le.minHeight = 26;

            MakeText(row, "Name", s.Item1, 15, MUTED, TextAnchor.MiddleLeft,
                new Vector2(0, 0), new Vector2(0.34f, 1), Vector2.zero, Vector2.zero);

            var track = MakePanel(row, "Track", new Color(0.15f, 0.14f, 0.18f, 1f),
                new Vector2(0.34f, 0.32f), new Vector2(1, 0.68f), Vector2.zero, new Vector2(-8, 0));

            MakePanel(track, "Fill", ACCENT,
                Vector2.zero, new Vector2(s.Item2 / 100f, 1f), Vector2.zero, Vector2.zero);
        }
    }

    private void BuildCampaignRows()
    {
        if (_campaignList == null) return;

        var campaigns = new[]
        {
            ("Очищение", "70–75", "Орден Света", "★☆☆☆"),
            ("Восхождение Света", "75–80", "Орден Света", "★★☆☆"),
            ("Восхождение Тьмы", "70–75", "Тёмный Орден", "★★☆☆"),
            ("Путь Баланса", "70–75", "Нейтральные", "★★☆☆"),
            ("Финальное пророчество", "90–99", "Нейтральные", "★★★★"),
        };

        foreach (var c in campaigns)
        {
            var row = MakePanel(_campaignList, "CampaignRow", new Color(0.13f, 0.115f, 0.155f, 1f),
                new Vector2(0, 1), new Vector2(1, 1), Vector2.zero, Vector2.zero);
            var le = row.gameObject.AddComponent<LayoutElement>();
            le.minHeight = 54;

            MakeText(row, "Name", c.Item1, 18, TEXT, TextAnchor.MiddleLeft,
                new Vector2(0, 0), new Vector2(0.46f, 1), new Vector2(14, 0), Vector2.zero);
            MakeText(row, "Levels", c.Item2, 15, GOLD, TextAnchor.MiddleLeft,
                new Vector2(0.46f, 0), new Vector2(0.62f, 1), Vector2.zero, Vector2.zero);
            MakeText(row, "Faction", c.Item3, 14, MUTED, TextAnchor.MiddleLeft,
                new Vector2(0.62f, 0), new Vector2(0.86f, 1), Vector2.zero, Vector2.zero);
            MakeText(row, "Diff", c.Item4, 14, GOLD, TextAnchor.MiddleRight,
                new Vector2(0.86f, 0), new Vector2(1, 1), Vector2.zero, new Vector2(-14, 0));

            string campaignName = c.Item1;
            var btn = row.gameObject.AddComponent<Button>();
            btn.targetGraphic = row.GetComponent<Image>();
            btn.onClick.AddListener(() => ShowToast($"Кампания «{campaignName}» — требуется 70+ уровень"));
        }
    }

    private void BuildGuildRows()
    {
        if (_guildWarList != null)
        {
            AddGuildRow(_guildWarList, "Умбра", "2 340 : 1 980");
            AddGuildRow(_guildWarList, "Пепельный легион", "980 : 1 120");
        }

        if (_guildAllyList != null)
        {
            AddGuildRow(_guildAllyList, "Серебряный клинок", "союз · с 12.10");
            AddGuildRow(_guildAllyList, "Дети Пустоты", "союз · с 03.11");
        }
    }

    private void AddGuildRow(RectTransform parent, string name, string meta)
    {
        var row = MakePanel(parent, "GuildRow", new Color(0.13f, 0.115f, 0.155f, 1f),
            new Vector2(0, 1), new Vector2(1, 1), Vector2.zero, Vector2.zero);
        var le = row.gameObject.AddComponent<LayoutElement>();
        le.minHeight = 42;

        MakePanel(row, "Accent", ACCENT,
            new Vector2(0, 0), new Vector2(0, 1), Vector2.zero, new Vector2(3, 0));

        MakeText(row, "Name", name, 16, TEXT, TextAnchor.MiddleLeft,
            new Vector2(0, 0), new Vector2(0.6f, 1), new Vector2(14, 0), Vector2.zero);
        MakeText(row, "Meta", meta, 15, GOLD, TextAnchor.MiddleRight,
            new Vector2(0.6f, 0), new Vector2(1, 1), Vector2.zero, new Vector2(-14, 0));
    }

    // ---------------------------------------------------------------- helpers

    private void BuildToast()
    {
        var toast = MakePanel(_root, "Toast", new Color(0.72f, 0.22f, 0.28f, 0.95f),
            new Vector2(0.5f, 1), new Vector2(0.5f, 1), new Vector2(-220, -140), new Vector2(220, -84));
        toast.pivot = new Vector2(0.5f, 1f);

        _toastText = MakeText(toast, "Text", "", 16, new Color(1f, 0.97f, 0.96f), TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, new Vector2(14, 0), new Vector2(-14, 0));

        _toast = toast.gameObject;
        _toast.SetActive(false);
    }

    private void ShowToast(string text)
    {
        if (_toast == null || _toastText == null) return;
        _toastText.text = text;
        _toast.SetActive(true);
        _toastTimer = 2.6f;
    }

    private void TogglePanel(GameObject panel)
    {
        if (panel == null) return;
        bool visible = panel.activeSelf;
        ClosePanels();
        panel.SetActive(!visible);
    }

    private void ClosePanels()
    {
        foreach (var p in new[] { _panelQuests, _panelInventory, _panelProfile,
                                  _panelCampaigns, _panelMap, _panelGuild, _panelSettings })
            if (p != null) p.SetActive(false);
    }

    public void ShowMainMenu()
    {
        SetScreen(_mainMenu);
        ClosePanels();
    }

    public void ShowCharacterSelect()
    {
        SetScreen(_charSelect);
    }

    public void ShowHud()
    {
        SetScreen(_hud);
        ClosePanels();
    }

    private void SetScreen(GameObject screen)
    {
        if (_mainMenu != null) _mainMenu.SetActive(false);
        if (_charSelect != null) _charSelect.SetActive(false);
        if (_hud != null) _hud.SetActive(false);

        if (screen != null) screen.SetActive(true);
    }
}
