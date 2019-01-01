# Wichtel-O-Mat / Secret Santa #
## Description ##
The purpose of this program is to solve the *Wichtel* problem. According to Wikipedia *Wichteln* is definied as follows:
> Secret Santa is a Western Christmas tradition in which members of a group or community are randomly assigned a person to whom they give a gift. The identity of the gift giver is a secret not to be revealed until after the gift is opened. 
(https://en.wikipedia.org/wiki/Secret_Santa)

The Wichtel-O-Mat randomly assigns each member of the group a partner. He or she has to make gift for that partner without revealing his or her identity.
Therefore, the Wichtel-O-Mat can notify each member via e-mail after calculating a solution. The assignments are not visible to the person who starts the program.

## Build it ##
The libcurl development files tls are needed. For Ubuntu 18.10, install `libcurl4-dev`.
To build it, a simple `make` is sufficient.

## Usage ##
Wichtel-O-Mat requires a text file as input that contains the participants. The format of that file is as follows:
```
John;john@doe.com;Phil,James
Phil;phil@doe.com;John
James;james@doe.com;John
```
The file must *not* include a header. The `conflicts` column is a CSV-separated list of `name`s.
Wichtel-O-Mat ensures that two participants are **not** assigned to each other. Moreover, it tries to find a solution 
that all assignments form a cycle, e.g. A-->B-->C-->A.
In addition to the input file the Wichtel-O-Mat accepts the following parameters:
```
--debug                     Enables debug output
--benchmark #Iterations     Enables the benchmark mode. This checks the accuracy of the random generator.
--ignore-conflicts          Ignores any conflicts described in the input file.
--send-email senderAddress  Disable any output about the solution. Notifies the members about the outcome. Implies --email-server
--email-server SMTPSERVER   The SMTP server to use. Mandatory if --send-email is used.
--email-port SMTPPORT       The port to use.
--email-user USERNAME       The username used for authentication.
--email-pass PASSWORD       The passwort used for authentication.
```
