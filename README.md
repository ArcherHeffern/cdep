# Description
IaC solution. Uses SSH under the hood

# Setup 
For each remote machine, enable ssh using:
`ssh-keygen`
`ssh-copy-id` 

# Syntax
__Remotes__  
[Remote name]  
remote_ip  
remote_username  
  
__Services__  
[Service Name]  
executable_location  
start_command  
remote_name  

# TODO
MINI NEXT: Abstract out to parser and types files
NEXT: Execute program
NEXT NEXT: Create type checking step
- Ability to stop deployed programs
- Stop previously ran programs when starting again
- Allow for pulling from git repositories
- Declaring of dependencies - This will allow for multithreading optimizations
- Hostname instead of ip address
- Tabs for nicer config files
- Convert to an actual parser
- Better checking if entries are file paths, ip addresses, etc
