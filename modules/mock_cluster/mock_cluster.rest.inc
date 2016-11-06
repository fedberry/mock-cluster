<?php
/**
 * @file
 * Mock Cluster REST API.
 */
 
/**
 * Return POST or PUT JSON descoded content.
 */
function mock_cluster_rest_request_content(&$received_json) {

  $received_json = file_get_contents('php://input', TRUE);

  if (!$data = json_decode($received_json, TRUE)) {
    return FALSE;
  }
  return $data;
}

/**
 * Auth handler.
 */
function mock_cluster_rest_auth() {
  $method = $_SERVER['REQUEST_METHOD'];
  $message = '';
  $received_json = '';
  switch($method){
    case 'POST':
      if(!$data = mock_cluster_rest_request_content($received_json)){
        return array('error' => true, 'message' => 'Bad request. No data provided.');
      }
      
      if(!isset($data['method'])) {
        return array('error' => true, 'message' => 'method value is required. options: register, token.');
      }
      switch($data['method']) {
        case 'register':
            if(!isset($data['identificationID'])){
              return array('error' => true, 'message' => 'identificationID value is required.');
            }
            if(!isset($data['arch'])){
              return array('error' => true, 'message' => 'arch value is required.');
            }
            
            $agent = db_select('mock_agents', 'ma')
              ->fields('ma')
              ->condition('identificationID', $data['identificationID'])
              ->execute()
              ->fetchAssoc();

            if(empty($agent)){
              $secret = backdrop_random_key();
              $token = backdrop_random_key();
              $record = array(
                'identificationID' => $data['identificationID'],
                'arch' => $data['arch'],
                'secret' => $secret,
                'token' => $token,
                'token_expire' => REQUEST_TIME + 86400, 
                'created' => REQUEST_TIME,
                'access' => REQUEST_TIME,
                'status' => 0,
                'uid' => 0
              );
              backdrop_write_record('mock_agents', $record); 
              return array(
                'token' => $token,
                'secret' => $secret
              );
            } else {
              return array('error' => true, 'message' => 'Agent with this identificationID is already registered.');
            }
          break;
        case 'token':
            if(!isset($data['identificationID'])){
              return array('error' => true, 'message' => 'identificationID value is required.');
            }
            if(!isset($data['arch'])){
              return array('error' => true, 'message' => 'arch value is required.');
            }

            $agent = db_select('mock_agents', 'ma')
              ->fields('ma')
              ->condition('identificationID', $data['identificationID'])
              ->condition('arch', $data['arch'])
              ->execute()
              ->fetchObject();

            if(empty($agent)){
              return array('error' => true, 'message' => 'Agent with this identificationID is not registered.');
            }else {
              if(!_is_rest_authorised($agent, $received_json)) {
                return array('error' => true, 'message' => 'Authorization failed.');
              }

              return array(
                'token' => $agent->token,
                'expire' => $agent->token_expire
              );
            }
          break;
        default: 
          return array('error' => true, 'message' => 'method should be register or token only.');
      }
  }
}

/**
 * Check authorisation.
 */
function _is_rest_authorised($agent, $received_json){

  $method = $_SERVER['REQUEST_METHOD'];
  //'PUT', 'SEARCH', 'PATCH', 'POST'
  switch($method){
    case 'PUT':
    case 'SEARCH':
    case 'PATCH':
    case 'POST':
        if (isset($_SERVER['HTTP_SIGNATURE'])) {
          list($algorithm, $expected_hash) = explode('=', $_SERVER['HTTP_SIGNATURE'], 2);
          $actual_hash = hash_hmac($algorithm, $received_json, $agent->secret);
          if ($expected_hash === $actual_hash) {
            return TRUE;
          } else {
            echo "expect:" . $expected_hash . " get" . $actual_hash . " data:" . $received_json ."\n";
          }
        }else {
          echo "No signature!!!!";
        }
      break;
    default:
      break;
  }
  return FALSE;
}

/**
 * Task handler.
 */
function mock_cluster_rest_task_by_id($task){
  $method = $_SERVER['REQUEST_METHOD'];
  $received_json = '';
  switch($method){
    case 'POST':
      $input = file_get_contents('php://input', TRUE);
      $filename = $_SERVER['HTTP_FILENAME'];
      $log_folder = 'private://mock_logs/' . $task->tid;

      file_prepare_directory($log_folder, FILE_CREATE_DIRECTORY);
      file_unmanaged_save_data($input, $log_folder . '/' . $filename, FILE_EXISTS_REPLACE);

      return(array('message' => $filename . ' uploaded'));
      
    case 'PUT':
      if(!$data = mock_cluster_rest_request_content($received_json)){
        return array('error' => true, 'message' => 'Bad request. No data provided.');
      }

      if(!isset($data['method'])){
        return array('error' => true, 'message' => 'method value is required.');
      }
      
      $token = $_SERVER['HTTP_TOKEN'];
      $agent = db_select('mock_agents', 'ma')
        ->fields('ma')
        ->condition('token', $token)
        ->execute()
        ->fetchObject();
        
      if(!$agent) {
        return array('error' => true, 'message' => 'Authorization failed. Token is not valid.');
      }else {
        if(!_is_rest_authorised($agent, $received_json)) {
          return array('error' => true, 'message' => 'Authorization failed. Signature is not valid'. $received_json);
        }
      }

      switch($data['method']){
        case 'delegate':
            if($task->status != MOCK_TASK_PENDING) {
              return array('error' => true, 'message' => 'Task is not available.'. print_r($_SERVER, true) . print_r($agent,true) . print_r($task,true));
            }
            
            $updated = db_update('mock_tasks')
              ->fields(array(
                'status' => MOCK_TASK_PROCESSING,
                'aid' => $agent->aid,
                'changed' => REQUEST_TIME)
              )
              ->condition('tid', $task->tid)
              ->condition('status', MOCK_TASK_PENDING)
              ->execute();
            
            if($updated) {
              return array('status' => 'pending');
            } else {
              array('error' => true, 'message' => 'Task is not available.');
            }
            return array('error' => true, 'message' => 'Task is not available.');

          break;
        case 'update':
            if($task->status != MOCK_TASK_PROCESSING && $task->aid != $agent->aid) {
              return array('error' => true, 'message' => 'Task is not available.');
            }
            
            mock_cluster_save_log($task,$data);

            return array('message' => 'Task updated.');
          break;
        case 'finished':
            if($task->status != MOCK_TASK_PROCESSING && $task->aid != $agent->aid) {
              return array('error' => true, 'message' => 'Task is not available.'. print_r($_SERVER, true) . print_r($agent,true) . print_r($task,true));
            }
            
            if(!isset($data['status'])) {
              return array('error' => true, 'message' => 'status is required');
            }
            
            $status = 0;
            switch($data['status']){
              case 'success':
                $status = MOCK_TASK_SUCCESS;
                break;
              case 'failure':
                $status = MOCK_TASK_FAILED;
                break;
              default:
                return array('error' => true, 'message' => 'status has to be success or failure');
            }
            
            mock_cluster_save_log($task,$data);
            
            
            $updated = db_update('mock_tasks')
              ->fields(array(
                'status' => $status,
                'changed' => REQUEST_TIME)
              )
              ->condition('tid', $task->tid)
              ->condition('status', MOCK_TASK_PROCESSING)
              ->execute();
            return array('message' => 'Task finished.');            
      }
      
    break;
  }
}

function mock_cluster_save_log($task, $data) {
  if(isset($data['log'])) {
    $log_folder = 'private://mock_logs/' . $task->tid;
    file_prepare_directory($log_folder, FILE_CREATE_DIRECTORY);
    
    file_unmanaged_save_data(base64_decode($data['log']), $log_folder . '/exec.log', FILE_EXISTS_REPLACE);
    
    file_unmanaged_save_data(base64_decode($data['mock']['build']), $log_folder . '/build.log', FILE_EXISTS_REPLACE);
    file_unmanaged_save_data(base64_decode($data['mock']['state']), $log_folder . '/state.log', FILE_EXISTS_REPLACE);
                  
    file_unmanaged_save_data(base64_decode($data['mock']['root']), $log_folder . '/root.log', FILE_EXISTS_REPLACE);
  }
}

/**
 * Tasks handler.
 */
function mock_cluster_rest_task(){
  $method = $_SERVER['REQUEST_METHOD'];
  $message = '';
  $received_json = '';
  switch($method){
    case 'SEARCH':
      if(!$data = mock_cluster_rest_request_content($received_json)){
        return array('error' => true, 'message' => 'Bad request. No data provided.');
      }
      if(!isset($data['identificationID'])){
        return array('error' => true, 'message' => 'identificationID value is required.');
      }
      if(!isset($data['arch'])){
        return array('error' => true, 'message' => 'arch value is required.');
      }

      $agent = db_select('mock_agents', 'ma')
        ->fields('ma')
        ->condition('identificationID', $data['identificationID'])
        ->condition('arch', $data['arch'])
        ->execute()
        ->fetchObject();

      if(empty($agent)){
        return array('error' => true, 'message' => 'Agent with this MAC is not registered.');
      }else {
        if(!_is_rest_authorised($agent, $received_json)) {
          return array('error' => true, 'message' => 'Authorization failed.');
        }
        
        $task = db_select('mock_tasks', 'mt')
          ->fields('mt')
          ->condition('arch', $agent->arch)
          ->condition('status', MOCK_TASK_PENDING)
          ->execute()
          ->fetchObject();
          
        return array( 'task' => $task);
      }
    break;
  }
}
