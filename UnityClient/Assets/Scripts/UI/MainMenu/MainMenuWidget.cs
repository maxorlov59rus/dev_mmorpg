using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

/// <summary>
/// Main Menu Widget - главное меню игры
/// </summary>
public class MainMenuWidget : MonoBehaviour
{
    [Header("UI References")]
    public Button playButton;
    public Button continueButton;
    public Button settingsButton;
    public Button exitButton;
    public GameObject mainMenuPanel;
    public GameObject optionsPanel;
    
    private bool isPaused = false;

    void Awake()
    {
        // Setup button listeners
        playButton.onClick.AddListener(OnPlayClicked);
        continueButton.onClick.AddListener(OnContinueClicked);
        settingsButton.onClick.AddListener(OnSettingsClicked);
        exitButton.onClick.AddListener(OnExitClicked);
        
        // Initially show main menu
        ShowMainMenu();
    }

    void ShowMainMenu()
    {
        mainMenuPanel.SetActive(true);
        optionsPanel.SetActive(false);
        Time.timeScale = 1f;
        isPaused = false;
    }

    void ShowOptions()
    {
        mainMenuPanel.SetActive(false);
        optionsPanel.SetActive(true);
    }

    void OnPlayClicked()
    {
        Debug.Log("Starting new game...");
        // Load first level or character creation
        Application.LoadLevel("CharacterCreation");
    }

    void OnContinueClicked()
    {
        Debug.Log("Continuing game...");
        // Load last save
    }

    void OnSettingsClicked()
    {
        ShowOptions();
    }

    void OnExitClicked()
    {
        Debug.Log("Exiting game...");
        Application.Quit();
    }

    void Update()
    {
        // Toggle menu with Escape key
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (!isPaused)
            {
                PauseGame();
            }
            else
            {
                ResumeGame();
            }
        }
    }

    void PauseGame()
    {
        isPaused = true;
        Time.timeScale = 0f;
        mainMenuPanel.SetActive(true);
    }

    void ResumeGame()
    {
        isPaused = false;
        Time.timeScale = 1f;
        mainMenuPanel.SetActive(false);
    }
}