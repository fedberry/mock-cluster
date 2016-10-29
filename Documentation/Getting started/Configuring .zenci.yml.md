# Configuring .zenci.yml

**.zenci.yml file structure is very simple and straight forward.**

You have 2 main sections to configure:
- deploy
- test

You don't have to have both. Depends on your needs, you can have only deploy or test.

#### Deploy

Deploy section contain deploy rules based on type of deploy. It could be **branch** or **pull_request**.
Section under name **branch** is wildcard and match any branch. However you can have **named** section that match your branch name. For example **master**:

```yaml
deploy:
  master:  #master branch deploy rules
  branch:  #all other branches deploy rules
  pull_request: #pull request deploy rules
```

You don't have to have all section defined. If there is no **pull_request** section, **ZenCI** do nothing on **PR** action. Same with **branch**. 

If you have only **named** branch section (like **master**), **ZenCI** will deploy code only on **push** to this repository.

Each section (branch, pull_request or named branch) has next important sections:

```yaml
    server: #deploy target 
    username: #username to connect with
    dir: #directory to deploy code
    env_vars: #list of variable: name .All this variables will be defined as Environment Variables.
    scripts: # scripts that we call on different stage of deploy
     init: # optional. We will call it only once, when code get cloned from repository
     before: #optional. We call it everytime before run `git clone` or `git pull`. You can use it to make a backup for example.
     after: #optional. We call it every time after deploy is done.
  webhooks:
    before_deploy: #optional. Full url to online available script. We call it before deploy. It can return Environment Variables.
    init: #optional. Full url to online available script. We call it after `git clone` happen. It can return Environment Variables.
    after_deploy:  #optional. Full url to online available script. We call it after `git clone` or `git pull` happen.
    fail_deploy: #optional. Full url to online available script. We call it if any errors happen during deploy. 
```

#### Test

Test section has a **TAG** on first level. It helps you to organize test by type. For example: php53, php70 etc..

```yaml
test:
  php53:
    branch:
    pull_request:
  php70
    branch:
    pull_request:
```

Inside **TAG** structure is similar to **deploy section:

```yaml
test:
  php53:
    branch:
      box: #box information
      dir:
      env_vars:
      scripts:
      webhooks:
      tests:
```

There is 2 new sections:
- box - See more details about it [here](http://docs.zen.ci/Tests/Test%20Boxes%20information)
- tests - list of scripts to run after code has been deployed on test box.

If you want to run tests on **your own box**, don't define box. Define **server** and **username** instead.


