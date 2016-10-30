<?php
/**
 * @file
 * Mock Agent pages provided by mock_cluster module.
 */

/**
 * Get task.
 */

function mock_cluster_task() {
  $mac = $_SERVER['HTTP_MAC'];
  $agent = mock_agent_load($mac);
  $task = db_select('mock_tasks', 'mt')
    ->fields('mt')
    ->condition('arch', $agent->arch)
    ->condition('status', MOCK_TASK_PENDING)
    ->execute()
    ->fetchObject();
  echo "URL:" . $task->url;
  echo "TASKID:" . $task->tid;
  exit();
}

/**
 * Implements register.
 */
function mock_cluster_register() {
    
  $received_json = file_get_contents('php://input', TRUE);


  if (!$data = backdrop_json_decode($received_json)) {
    backdrop_add_http_header('Status', '403 Empty or broken data received');
    exit('Empty or broken data received.');
  }

  if(!isset($data['secret'])){
    backdrop_add_http_header('Status', '403 Secret is not provided');
    exit('Secret is not provided.');
  }

  if(!isset($data['MAC'])){
    backdrop_add_http_header('Status', '403 MAC address is not provided');
    exit('MAC address is not provided.');
  }

  if(!isset($data['arch'])){
    backdrop_add_http_header('Status', '403 ARCH is not provided');
    exit('ARCH is not provided.');
  }

  $agent = db_select('mock_agents', 'ma')
    ->fields('ma')
    ->condition('MAC', $data['MAC'])
    ->execute()
    ->fetchAssoc();
    
  if(empty($agent)){
    $record = array(
      'MAC' => $data['MAC'],
      'arch' => $data['arch'],
      'secret' => $data['secret'],
      'created' => REQUEST_TIME,
      'access' => REQUEST_TIME,
      'status' => 0,
      'uid' => 0
    );
    backdrop_write_record('mock_agents', $record);
    backdrop_add_http_header('Status', '200 Agent registered');
    exit('Agent registered.');
  } else {
    backdrop_add_http_header('Status', '403 Agent already registered');
    exit('Agent already registered.');
  }
}

