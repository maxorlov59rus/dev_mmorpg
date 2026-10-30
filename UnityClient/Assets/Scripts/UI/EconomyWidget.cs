using UnityEngine;
using DarkMatter.Network;
using System;

namespace DM.Client
{
    public class EconomyWidget : MonoBehaviour
    {
        public MMONetworkClient networkClient;

        public void SetNetworkClient(MMONetworkClient client)
        {
            networkClient = client;
        }

        public void UpdateCurrency(byte currencyType, uint amount)
        {
            Debug.Log($"Currency update: type={currencyType}, amount={amount}");
            // TODO: update UI
        }
    }
}
