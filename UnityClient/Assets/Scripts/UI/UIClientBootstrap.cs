using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
using System.Collections.Generic;

/// <summary>
/// Runtime UI bootstrap: builds the client screens with uGUI so the project runs without prefabs.
/// Screens: main menu -> character creation -> HUD (chat, quest log, profile, campaigns).
/// </summary>
public class UIClientBootstrap : MonoBehaviour
{
    public static UIClientBootstrap Instance { get; private set; }

    [Header("Theme")]
    public Color background = new Color(0.06f, 0.05f, 0.08f, 1f);
    public Color panel = new Color(0.11f, 0.10f, 0.14f, 0.97f);
    public Color accent = new Color(0.72f, 0.22f, 0.28f, 1f);
    public Color accentLight = new Color(0.94f, 0.80f, 0.55f, 1f);
    public Color textPrimary = new Color(0.93f, 0.92f, 0.95f, 1f);
    public Color textMuted = new Color(0.62f, 0.60f, 0.66f, 1f);

    private Canvas _canvas;
    private GameObject _mainMenu;
    private GameObject _hud;
    private RectTransform _screenRoot;

    private Text _hudLevel;
    private Text _hudGold;
    private Text _hudFaction;
    private Text _hudZone;
    private Slider _hpSlider;
    private Slider _mpSlider;
    private Text _hpLabel;
    private Text _mpLabel;

    private RectTransform _chatContent;
    private InputField _chatInput;
    private Text _channelLabel;
    private string _channel = "Общий";

    private GameObject _questPanel;
    private GameObject _profilePanel;
    private GameObject _campaignPanel;

    private PlayerProfile _profile;

    void Awake()
    {
        Instance = this;
        EnsureEventSystem();
        EnsureCamera();
        BuildCanvas();
        _profile = new PlayerProfile { guid = 1, name = "Странник", level = 70, faction_id = 1, class_id = 3, race_id = 1, gold = 1250, spirit = 40 };
        BuildMainMenu();
        BuildHud();
        ShowMainMenu();
    }

    void Update()
    {
        if (_hud != null && _hud.activeSelf)
        {
            HandleHotkeys();
        }
    }

    private void HandleHotkeys()
    {
        if (Input.GetKeyDown(KeyCode.L)) TogglePanel(_questPanel);
        if (Input.GetKeyDown(KeyCode.C)) TogglePanel(_profilePanel);
        if (Input.GetKeyDown(KeyCode.K)) TogglePanel(_campaignPanel);
        if (Input.GetKeyDown(KeyCode.Escape) && _chatInput != null && _chatInput.isFocused == false)
        {
            ClosePanels();
        }
    }

    private void TogglePanel(GameObject panel)
    {
        if (panel == null) return;
        bool next = !panel.activeSelf;
        ClosePanels();
        panel.SetActive(next);
    }

    private void ClosePanels()
    {
        if (_questPanel != null) _questPanel.SetActive(false);
        if (_profilePanel != null) _profilePanel.SetActive(false);
        if (_campaignPanel != null) _campaignPanel.SetActive(false);
    }

    // ---------- infrastructure ----------

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
        cam.backgroundColor = background;
        go.tag = "MainCamera";
    }

    private void BuildCanvas()
    {
        var go = new GameObject("UICanvas");
        _canvas = go.AddComponent<Canvas>();
        _canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        var scaler = go.AddComponent<CanvasScaler>();
        scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1920, 1080);
        scaler.matchWidthOrHeight = 0.5f;
        go.AddComponent<GraphicRaycaster>();

        _screenRoot = go.GetComponent<RectTransform>();
    }

    private RectTransform MakePanel(Transform parent, string name, Color color, Vector2 anchorMin, Vector2 anchorMax, Vector2 offsetMin, Vector2 offsetMax)
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

    private Text MakeText(Transform parent, string name, string value, int size, Color color, TextAnchor anchor, Vector2 anchorMin, Vector2 anchorMax, Vector2 offsetMin, Vector2 offsetMax)
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
        t.fontSize = size;
        t.color = color;
        t.alignment = anchor;
        t.font = GetDefaultFont();
        t.horizontalOverflow = HorizontalWrapMode.Wrap;
        t.verticalOverflow = VerticalWrapMode.Overflow;
        return t;
    }

    private static Font _font;
    private static Font GetDefaultFont()
    {
        if (_font != null) return _font;
        _font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
        if (_font == null) _font = Resources.GetBuiltinResource<Font>("Arial.ttf");
        return _font;
    }

    private UnityEngine.UI.Button MakeButton(Transform parent, string name, string label, Vector2 size, Vector2 position, Color bg, System.Action onClick)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Image), typeof(UnityEngine.UI.Button));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.sizeDelta = size;
        rt.anchoredPosition = position;

        go.GetComponent<Image>().color = bg;
        var btn = go.GetComponent<UnityEngine.UI.Button>();
        var colors = btn.colors;
        colors.highlightedColor = new Color(1f, 1f, 1f, 0.85f);
        colors.pressedColor = new Color(0.8f, 0.8f, 0.8f, 1f);
        btn.colors = colors;

        MakeText(rt, "Label", label, 22, textPrimary, TextAnchor.MiddleCenter,
            Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);

        if (onClick != null) btn.onClick.AddListener(() => onClick());
        return btn;
    }

    private Slider MakeSlider(Transform parent, string name, Color fill, Vector2 size, Vector2 position, float value)
    {
        var go = new GameObject(name, typeof(RectTransform), typeof(Slider));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(parent, false);
        rt.sizeDelta = size;
        rt.anchoredPosition = position;

        var bgGo = new GameObject("Background", typeof(RectTransform), typeof(Image));
        var bgRt = bgGo.GetComponent<RectTransform>();
        bgRt.SetParent(rt, false);
        bgRt.anchorMin = Vector2.zero;
        bgRt.anchorMax = Vector2.one;
        bgRt.offsetMin = Vector2.zero;
        bgRt.offsetMax = Vector2.zero;
        bgGo.GetComponent<Image>().color = new Color(0.18f, 0.17f, 0.2f, 1f);

        var fillArea = new GameObject("Fill Area", typeof(RectTransform));
        var fillAreaRt = fillArea.GetComponent<RectTransform>();
        fillAreaRt.SetParent(rt, false);
        fillAreaRt.anchorMin = Vector2.zero;
        fillAreaRt.anchorMax = Vector2.one;
        fillAreaRt.offsetMin = new Vector2(2, 2);
        fillAreaRt.offsetMax = new Vector2(-2, -2);

        var fillGo = new GameObject("Fill", typeof(RectTransform), typeof(Image));
        var fillRt = fillGo.GetComponent<RectTransform>();
        fillRt.SetParent(fillAreaRt, false);
        fillRt.anchorMin = Vector2.zero;
        fillRt.anchorMax = Vector2.one;
        fillRt.offsetMin = Vector2.zero;
        fillRt.offsetMax = Vector2.zero;
        fillGo.GetComponent<Image>().color = fill;

        var slider = go.GetComponent<Slider>();
        slider.fillRect = fillRt;
        slider.targetGraphic = fillGo.GetComponent<Image>();
        slider.direction = Slider.Direction.LeftToRight;
        slider.minValue = 0f;
        slider.maxValue = 100f;
        slider.value = value;
        return slider;
    }

    // ---------- main menu ----------

    private void BuildMainMenu()
    {
        var root = MakePanel(_screenRoot, "MainMenu", background, Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
        _mainMenu = root.gameObject;

        var vignette = MakePanel(root, "Vignette", new Color(0.35f, 0.06f, 0.10f, 0.35f),
            new Vector2(0f, 0.55f), new Vector2(1f, 1f), Vector2.zero, Vector2.zero);

        MakeText(root, "Title", "ЧЁРНАЯ МАТЕРИЯ", 96, accentLight, TextAnchor.MiddleCenter,
            new Vector2(0f, 0.62f), new Vector2(1f, 0.82f), Vector2.zero, Vector2.zero);

        MakeText(root, "Subtitle", "BLACK MATTER  ·  MMORPG", 26, textMuted, TextAnchor.MiddleCenter,
            new Vector2(0f, 0.55f), new Vector2(1f, 0.62f), Vector2.zero, Vector2.zero);

        MakeText(root, "Tagline", "Три силы. Один раскол. Мир на грани Заражения.", 22, textMuted, TextAnchor.MiddleCenter,
            new Vector2(0f, 0.48f), new Vector2(1f, 0.54f), Vector2.zero, Vector2.zero);

        var buttons = MakePanel(root, "MenuButtons", new Color(0, 0, 0, 0),
            new Vector2(0.5f, 0.5f), new Vector2(0.5f, 0.5f), new Vector2(-170, -170), new Vector2(170, 130));

        MakeButton(buttons, "PlayButton", "Играть", new Vector2(340, 56), new Vector2(0, 96), accent, () => ShowHud());
        MakeButton(buttons, "InfoButton", "О мире", new Vector2(340, 56), new Vector2(0, 28), panel, () => ShowWorldInfo());
        MakeButton(buttons, "QuitButton", "Выход", new Vector2(340, 56), new Vector2(0, -40), panel, QuitGame);

        MakeText(root, "Footer", "v0.1 · прототип клиента · Unity 7", 16, textMuted, TextAnchor.MiddleCenter,
            new Vector2(0f, 0.02f), new Vector2(1f, 0.06f), Vector2.zero, Vector2.zero);
    }

    private Text _infoText;

    private void ShowWorldInfo()
    {
        if (_infoText != null) { _infoText.gameObject.SetActive(!_infoText.gameObject.activeSelf); return; }

        _infoText = MakeText(_mainMenu.transform, "WorldInfo",
            "Орден Света — порядок и искупление. Столица: Ательгард.\n" +
            "Тёмный Орден — власть и контроль. Столица: Умбра.\n" +
            "Нейтральные — свобода и исследование. Столица: Грейвхейвен.\n\n" +
            "Кампании 70–99 открываются с выбором фракции.\n" +
            "Заражение, Разрыв и Великая охота меняют мир каждую неделю.",
            20, textMuted, TextAnchor.UpperCenter,
            new Vector2(0.18f, 0.10f), new Vector2(0.82f, 0.42f), Vector2.zero, Vector2.zero);
    }

    private void QuitGame()
    {
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
    }

    // ---------- HUD ----------

    private void BuildHud()
    {
        var root = MakePanel(_screenRoot, "HUD", new Color(0, 0, 0, 0), Vector2.zero, Vector2.one, Vector2.zero, Vector2.zero);
        _hud = root.gameObject;

        BuildTopBar(root);
        BuildVitals(root);
        BuildChat(root);
        BuildHotkeyBar(root);
        BuildQuestPanel(root);
        BuildProfilePanel(root);
        BuildCampaignPanel(root);
    }

    private void BuildTopBar(Transform root)
    {
        var bar = MakePanel(root, "TopBar", panel,
            new Vector2(0f, 1f), new Vector2(1f, 1f), new Vector2(0, -64), Vector2.zero);

        var name = MakeText(bar, "PlayerName", "Странник", 24, accentLight, TextAnchor.MiddleLeft,
            new Vector2(0f, 0f), new Vector2(0.2f, 1f), new Vector2(24, 0), Vector2.zero);

        _hudLevel = MakeText(bar, "Level", "70 ур.", 20, textPrimary, TextAnchor.MiddleLeft,
            new Vector2(0.2f, 0f), new Vector2(0.3f, 1f), Vector2.zero, Vector2.zero);

        _hudFaction = MakeText(bar, "Faction", "Орден Света", 20, textPrimary, TextAnchor.MiddleLeft,
            new Vector2(0.3f, 0f), new Vector2(0.5f, 1f), Vector2.zero, Vector2.zero);

        _hudZone = MakeText(bar, "Zone", "Ательгард", 20, textMuted, TextAnchor.MiddleRight,
            new Vector2(0.5f, 0f), new Vector2(0.8f, 1f), Vector2.zero, Vector2.zero);

        _hudGold = MakeText(bar, "Gold", "1250 золота", 20, accentLight, TextAnchor.MiddleRight,
            new Vector2(0.8f, 0f), new Vector2(1f, 1f), Vector2.zero, new Vector2(-24, 0));
    }

    private void BuildVitals(Transform root)
    {
        var box = MakePanel(root, "Vitals", panel,
            new Vector2(0f, 0f), new Vector2(0f, 0f), new Vector2(20, 20), new Vector2(360, 150));

        _hpSlider = MakeSlider(box, "HealthBar", new Color(0.72f, 0.20f, 0.24f), new Vector2(320, 26), new Vector2(20, 96), 82f);
        _mpSlider = MakeSlider(box, "ManaBar", new Color(0.26f, 0.44f, 0.82f), new Vector2(320, 26), new Vector2(20, 56), 64f);

        _hpLabel = MakeText(box, "HealthText", "HP 820 / 1000", 16, textPrimary, TextAnchor.MiddleCenter,
            new Vector2(0, 0.62f), new Vector2(1, 0.8f), Vector2.zero, Vector2.zero);

        _mpLabel = MakeText(box, "ManaText", "MP 640 / 1000", 16, textPrimary, TextAnchor.MiddleCenter,
            new Vector2(0, 0.35f), new Vector2(1, 0.53f), Vector2.zero, Vector2.zero);

        MakeText(box, "Info", "Орден Света  ·  уровень 70", 16, textMuted, TextAnchor.MiddleCenter,
            new Vector2(0, 0f), new Vector2(1, 0.3f), Vector2.zero, Vector2.zero);
    }

    private void BuildChat(Transform root)
    {
        var box = MakePanel(root, "Chat", panel,
            new Vector2(0f, 0f), new Vector2(0f, 0f), new Vector2(20, 190), new Vector2(620, 520));

        _channelLabel = MakeText(box, "ChannelLabel", "Общий", 18, accentLight, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(0.5f, 1), new Vector2(14, -34), new Vector2(0, 0));

        MakeButton(box, "ChannelButton", "Сменить канал", new Vector2(150, 24), new Vector2(-90, -22), new Color(0.18f,0.17f,0.22f), CycleChannel);

        var viewport = MakePanel(box, "Viewport", new Color(0.07f, 0.06f, 0.09f, 1f),
            new Vector2(0, 0), new Vector2(1, 1), new Vector2(12, 76), new Vector2(-12, -54));

        _chatContent = MakePanel(viewport, "Content", new Color(0, 0, 0, 0),
            new Vector2(0, 1), new Vector2(1, 1), Vector2.zero, Vector2.zero);

        var inputGo = new GameObject("ChatInput", typeof(RectTransform), typeof(Image), typeof(InputField));
        var inputRt = inputGo.GetComponent<RectTransform>();
        inputRt.SetParent(box, false);
        inputRt.anchorMin = new Vector2(0, 0);
        inputRt.anchorMax = new Vector2(1, 0);
        inputRt.offsetMin = new Vector2(12, 12);
        inputRt.offsetMax = new Vector2(-12, 58);
        inputGo.GetComponent<Image>().color = new Color(0.16f, 0.15f, 0.19f, 1f);

        _chatInput = inputGo.GetComponent<InputField>();
        var textGo = new GameObject("Text", typeof(RectTransform), typeof(Text));
        var textRt = textGo.GetComponent<RectTransform>();
        textRt.SetParent(inputRt, false);
        textRt.anchorMin = Vector2.zero;
        textRt.anchorMax = Vector2.one;
        textRt.offsetMin = new Vector2(12, 4);
        textRt.offsetMax = new Vector2(-12, -4);
        var text = textGo.GetComponent<Text>();
        text.font = GetDefaultFont();
        text.fontSize = 18;
        text.color = textPrimary;
        text.alignment = TextAnchor.MiddleLeft;
        text.supportRichText = false;

        var placeholderGo = new GameObject("Placeholder", typeof(RectTransform), typeof(Text));
        var phRt = placeholderGo.GetComponent<RectTransform>();
        phRt.SetParent(inputRt, false);
        phRt.anchorMin = Vector2.zero;
        phRt.anchorMax = Vector2.one;
        phRt.offsetMin = new Vector2(12, 4);
        phRt.offsetMax = new Vector2(-12, -4);
        var ph = placeholderGo.GetComponent<Text>();
        ph.text = "Написать сообщение… (Enter)";
        ph.font = GetDefaultFont();
        ph.fontSize = 18;
        ph.color = textMuted;
        ph.alignment = TextAnchor.MiddleLeft;

        _chatInput.textComponent = text;
        _chatInput.placeholder = ph;
        _chatInput.lineType = InputField.LineType.SingleLine;
        _chatInput.onEndEdit.AddListener(OnChatSubmit);

        AddChatLine("Система", "Добро пожаловать в Чёрную Материю.", accentLight);
        AddChatLine("Система", "L — журнал квестов, C — профиль, K — кампании.", textMuted);
        AddChatLine("Виксис", "Свет не прощает отступников, но принимает кающихся.", new Color(0.85f,0.80f,0.60f));
    }

    private void CycleChannel()
    {
        var channels = new[] { "Общий", "Отряд", "Фракция", "Торговля", "Шёпот" };
        int i = System.Array.IndexOf(channels, _channel);
        _channel = channels[(i + 1) % channels.Length];
        if (_channelLabel != null) _channelLabel.text = _channel;
    }

    private void OnChatSubmit(string value)
    {
        if (string.IsNullOrWhiteSpace(value)) return;
        AddChatLine(_profile != null ? _profile.name : "Игрок", value, textPrimary);
        if (GameManager.Instance != null && GameManager.Instance.ChatManager != null)
        {
            GameManager.Instance.ChatManager.SendMessage(_profile != null ? _profile.guid : 0, value, _channel, 0);
        }
        _chatInput.text = "";
        _chatInput.ActivateInputField();
    }

    private void AddChatLine(string author, string message, Color authorColor)
    {
        if (_chatContent == null) return;

        var go = new GameObject("Line", typeof(RectTransform), typeof(Text));
        var rt = go.GetComponent<RectTransform>();
        rt.SetParent(_chatContent, false);
        rt.anchorMin = new Vector2(0, 1);
        rt.anchorMax = new Vector2(1, 1);
        rt.pivot = new Vector2(0, 1);

        int count = _chatContent.childCount;
        rt.anchoredPosition = new Vector2(0, -8 - count * 28);
        rt.sizeDelta = new Vector2(0, 26);

        var t = go.GetComponent<Text>();
        t.font = GetDefaultFont();
        t.fontSize = 17;
        t.color = textPrimary;
        t.alignment = TextAnchor.MiddleLeft;
        t.horizontalOverflow = HorizontalWrapMode.Wrap;
        t.supportRichText = true;
        t.text = $"<color=#{ColorUtility.ToHtmlStringRGB(authorColor)}>[{author}]</color> {message}";

        var contentRt = _chatContent;
        contentRt.sizeDelta = new Vector2(contentRt.sizeDelta.x, 8 + (count + 1) * 28);
    }

    private void BuildHotkeyBar(Transform root)
    {
        var bar = MakePanel(root, "HotkeyBar", panel,
            new Vector2(0.5f, 0f), new Vector2(0.5f, 0f), new Vector2(-330, 16), new Vector2(330, 84));

        MakeButton(bar, "QuestBtn", "L · Квесты", new Vector2(150, 46), new Vector2(-240, 0), new Color(0.18f,0.17f,0.22f), () => TogglePanel(_questPanel));
        MakeButton(bar, "ProfileBtn", "C · Профиль", new Vector2(150, 46), new Vector2(-80, 0), new Color(0.18f,0.17f,0.22f), () => TogglePanel(_profilePanel));
        MakeButton(bar, "CampaignBtn", "K · Кампании", new Vector2(150, 46), new Vector2(80, 0), new Color(0.18f,0.17f,0.22f), () => TogglePanel(_campaignPanel));
        MakeButton(bar, "MenuBtn", "Выход в меню", new Vector2(170, 46), new Vector2(250, 0), accent, ShowMainMenu);
    }

    private void BuildQuestPanel(Transform root)
    {
        var panelRt = MakePanel(root, "QuestPanel", panel,
            new Vector2(0.5f, 0.5f), new Vector2(0.5f, 0.5f), new Vector2(-360, -260), new Vector2(360, 260));
        _questPanel = panelRt.gameObject;

        MakeText(panelRt, "Title", "Журнал квестов", 30, accentLight, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(24, -56), new Vector2(-24, -12));

        MakeButton(panelRt, "Close", "×", new Vector2(40, 40), new Vector2(-28, -28), accent, () => _questPanel.SetActive(false));

        var entries = new[]
        {
            ("Очищение окраин", "Очистите заражённые земли Ательгарда.", "3 / 8 зон"),
            ("Свет над руинами", "Восстановите солнечный храм.", "1 / 3 реликвии"),
            ("Голос Умбры", "Выслушайте Великого Мага Таллона.", "Готово"),
        };

        float y = -100;
        foreach (var e in entries)
        {
            var row = MakePanel(panelRt, "QuestRow", new Color(0.16f, 0.15f, 0.19f, 1f),
                new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, y - 96), new Vector2(-20, y));

            MakeText(row, "Name", e.Item1, 22, textPrimary, TextAnchor.UpperLeft,
                new Vector2(0, 0.45f), new Vector2(1, 1), new Vector2(16, 0), new Vector2(-16, -8));
            MakeText(row, "Desc", e.Item2, 17, textMuted, TextAnchor.UpperLeft,
                new Vector2(0, 0.2f), new Vector2(1, 0.55f), new Vector2(16, 0), new Vector2(-16, 0));
            MakeText(row, "Progress", e.Item3, 17, accentLight, TextAnchor.LowerRight,
                new Vector2(0.5f, 0), new Vector2(1, 0.25f), new Vector2(0, 6), new Vector2(-16, 0));

            y -= 112;
        }
    }

    private void BuildProfilePanel(Transform root)
    {
        var panelRt = MakePanel(root, "ProfilePanel", panel,
            new Vector2(0.5f, 0.5f), new Vector2(0.5f, 0.5f), new Vector2(-280, -300), new Vector2(280, 300));
        _profilePanel = panelRt.gameObject;

        MakeText(panelRt, "Title", "Профиль", 30, accentLight, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(24, -56), new Vector2(-24, -12));

        MakeButton(panelRt, "Close", "×", new Vector2(40, 40), new Vector2(-28, -28), accent, () => _profilePanel.SetActive(false));

        var avatar = MakePanel(panelRt, "Avatar", new Color(0.3f, 0.12f, 0.16f, 1f),
            new Vector2(0.5f, 1f), new Vector2(0.5f, 1f), new Vector2(-70, -230), new Vector2(70, -90));

        MakeText(panelRt, "AvatarMark", "◈", 64, accentLight, TextAnchor.MiddleCenter,
            new Vector2(0.5f, 1f), new Vector2(0.5f, 1f), new Vector2(-70, -230), new Vector2(70, -90));

        var lines = new[]
        {
            "Странник",
            "Уровень 70 · Жрец Света",
            "Раса: человек · Фракция: Орден Света",
            "Золото: 1250 · Дух: 40",
            "Титул: Кающийся",
        };

        float y = -250;
        foreach (var line in lines)
        {
            MakeText(panelRt, "Line", line, 20, textPrimary, TextAnchor.MiddleLeft,
                new Vector2(0, 1), new Vector2(1, 1), new Vector2(32, y - 34), new Vector2(-32, y));
            y -= 42;
        }

        var stats = new[] { ("Сила", 45f), ("Ловкость", 60f), ("Интеллект", 88f), ("Выносливость", 70f), ("Духовность", 95f) };
        foreach (var s in stats)
        {
            MakeText(panelRt, "StatName", s.Item1, 17, textMuted, TextAnchor.MiddleLeft,
                new Vector2(0, 1), new Vector2(0.35f, 1), new Vector2(32, y - 24), new Vector2(0, y));
            MakeSlider(panelRt, "StatBar", accent, new Vector2(200, 14), new Vector2(40, y - 12), s.Item2);
            y -= 40;
        }
    }

    private void BuildCampaignPanel(Transform root)
    {
        var panelRt = MakePanel(root, "CampaignPanel", panel,
            new Vector2(0.5f, 0.5f), new Vector2(0.5f, 0.5f), new Vector2(-400, -280), new Vector2(400, 280));
        _campaignPanel = panelRt.gameObject;

        MakeText(panelRt, "Title", "Кампании 70+", 30, accentLight, TextAnchor.MiddleLeft,
            new Vector2(0, 1), new Vector2(1, 1), new Vector2(24, -56), new Vector2(-24, -12));

        MakeButton(panelRt, "Close", "×", new Vector2(40, 40), new Vector2(-28, -28), accent, () => _campaignPanel.SetActive(false));

        var campaigns = new[]
        {
            ("Очищение", "70–75", "Орден Света", "★☆☆☆", "Ательгард"),
            ("Восхождение Света", "75–80", "Орден Света", "★★☆☆", "Храм Солнца"),
            ("Восхождение Тьмы", "70–75", "Тёмный Орден", "★★☆☆", "Умбра"),
            ("Путь Баланса", "70–75", "Нейтральные", "★★☆☆", "Грейвхейвен"),
        };

        float y = -100;
        foreach (var c in campaigns)
        {
            var row = MakePanel(panelRt, "CampaignRow", new Color(0.16f, 0.15f, 0.19f, 1f),
                new Vector2(0, 1), new Vector2(1, 1), new Vector2(20, y - 92), new Vector2(-20, y));

            MakeText(row, "Name", c.Item1, 22, textPrimary, TextAnchor.MiddleLeft,
                new Vector2(0, 0.5f), new Vector2(0.4f, 1f), new Vector2(16, 0), Vector2.zero);
            MakeText(row, "Levels", c.Item2, 18, accentLight, TextAnchor.MiddleLeft,
                new Vector2(0.4f, 0.5f), new Vector2(0.55f, 1f), Vector2.zero, Vector2.zero);
            MakeText(row, "Faction", c.Item3, 17, textMuted, TextAnchor.MiddleLeft,
                new Vector2(0.55f, 0.5f), new Vector2(0.8f, 1f), Vector2.zero, Vector2.zero);
            MakeText(row, "Diff", c.Item4, 17, textMuted, TextAnchor.MiddleLeft,
                new Vector2(0, 0.05f), new Vector2(0.5f, 0.5f), new Vector2(16, 0), Vector2.zero);
            MakeText(row, "Place", c.Item5, 17, textMuted, TextAnchor.MiddleRight,
                new Vector2(0.5f, 0.05f), new Vector2(1, 0.5f), Vector2.zero, new Vector2(-16, 0));

            y -= 108;
        }
    }

    // ---------- screens ----------

    public void ShowMainMenu()
    {
        if (_mainMenu != null) _mainMenu.SetActive(true);
        if (_hud != null) _hud.SetActive(false);
    }

    public void ShowHud()
    {
        if (_mainMenu != null) _mainMenu.SetActive(false);
        if (_hud != null) _hud.SetActive(true);
        ClosePanels();
    }
}
