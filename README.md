# Initial Knowledge in Cybersecurity

This repository documents my first steps, studies, and practical tools focused on Cybersecurity, Automation, and Networking. The goal is to understand the fundamentals of operating systems and network protocols by building security tools from scratch.

## Developed Tools

So far, the repository contains reconnaissance (Recon) scripts focused on infrastructure:

*   **Ping Sweeper (`ip_scanner.sh`):** A network mapper (Host Discovery) developed in pure Shell Script. It uses parallelism (background processes) to sweep an entire `/24` subnet in seconds, filtering active responses with `grep`, `cut`, and `tr`.
*   **Native Port Scanner (`port_scanner.sh`):** A TCP port scanner that does not rely on Nmap. Built 100% in Bash, it uses the `/dev/tcp` pseudo-device to perform network connections (Full Connect) on common ports, applying timers to avoid hanging against Firewalls.

## Technologies and Applied Concepts

*   **Language:** Shell Script (Bash) in a Linux environment (WSL).
*   **Networking:** TCP/IP concepts, ICMP (Ping), ports, and sockets.
*   **Linux CLI:** Stream manipulation (Pipelines `|`), data redirection (`>`, `>>`, `/dev/null`), positional parameters (`$1`, `$2`), and Subshells.

---
*Studies in progress.*
