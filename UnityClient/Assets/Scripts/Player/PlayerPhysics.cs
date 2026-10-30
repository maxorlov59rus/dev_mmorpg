// PlayerPhysics.cs - Player movement and physics controller for Unity
using UnityEngine;

namespace DM.Client
{
    /// <summary>
    /// Controls player movement, physics, and camera.
    /// </summary>
    [RequireComponent(typeof(CharacterController))]
    public class PlayerPhysics : MonoBehaviour
    {
        [Header("Movement")]
        public float walkSpeed = 5f;
        public float runSpeed = 8f;
        public float jumpHeight = 1.5f;
        public float gravity = -9.81f;

        [Header("Camera")]
        public Transform cameraTransform;
        public float mouseSensitivity = 2f;
        public float cameraDistance = 5f;
        public float cameraHeight = 2f;

        [Header("Ground Check")]
        public Transform groundCheck;
        public float groundDistance = 0.4f;
        public LayerMask groundMask;

        private CharacterController _controller;
        private Vector3 _velocity;
        private bool _isGrounded;
        private float _xRotation = 0f;
        private float _yRotation = 0f;

        void Start()
        {
            _controller = GetComponent<CharacterController>();

            if (cameraTransform == null)
            {
                var cam = Camera.main;
                if (cam != null)
                    cameraTransform = cam.transform;
            }

            UnityEngine.Cursor.lockState = CursorLockMode.Locked;
            UnityEngine.Cursor.visible = false;
        }

        void Update()
        {
            // Ground check
            _isGrounded = Physics.CheckSphere(groundCheck.position, groundDistance, groundMask);

            if (_isGrounded && _velocity.y < 0)
            {
                _velocity.y = -2f;
            }

            // Movement input
            float horizontal = Input.GetAxis("Horizontal");
            float vertical = Input.GetAxis("Vertical");

            Vector3 move = transform.right * horizontal + transform.forward * vertical;
            float speed = Input.GetKey(KeyCode.LeftShift) ? runSpeed : walkSpeed;
            _controller.Move(move * speed * Time.deltaTime);

            // Jump
            if (Input.GetButtonDown("Jump") && _isGrounded)
            {
                _velocity.y = Mathf.Sqrt(jumpHeight * -2f * gravity);
            }

            // Gravity
            _velocity.y += gravity * Time.deltaTime;
            _controller.Move(_velocity * Time.deltaTime);

            // Camera rotation
            float mouseX = Input.GetAxis("Mouse X") * mouseSensitivity;
            float mouseY = Input.GetAxis("Mouse Y") * mouseSensitivity;

            _yRotation += mouseX;
            _xRotation -= mouseY;
            _xRotation = Mathf.Clamp(_xRotation, -40f, 80f);

            transform.rotation = Quaternion.Euler(0f, _yRotation, 0f);

            if (cameraTransform != null)
            {
                cameraTransform.rotation = Quaternion.Euler(_xRotation, _yRotation, 0f);
                cameraTransform.position = transform.position + Vector3.up * cameraHeight - cameraTransform.forward * cameraDistance;
            }

            // Toggle cursor
            if (Input.GetKeyDown(KeyCode.Escape))
            {
                if (UnityEngine.Cursor.lockState == CursorLockMode.Locked)
                {
                    UnityEngine.Cursor.lockState = CursorLockMode.None;
                    UnityEngine.Cursor.visible = true;
                }
                else
                {
                    UnityEngine.Cursor.lockState = CursorLockMode.Locked;
                    UnityEngine.Cursor.visible = false;
                }
            }
        }

        void OnDrawGizmosSelected()
        {
            if (groundCheck != null)
            {
                Gizmos.color = Color.red;
                Gizmos.DrawWireSphere(groundCheck.position, groundDistance);
            }
        }
    }
}