<?php
/**
 * Implements of hook_b_command().
 */
function github_pages_b_command() {
  $items = array();

  $items['github-pages-update'] = array(
    'bootstrap' => B_BOOTSTRAP_FULL,
    'description' => 'Update github pages aliases.',
    'callback' => 'github_pages_command_update',
  );
  return $items;
}

/**
 * @param array $arguments
 *  cache clear and optional type
 * @param $options
 *  todo: are we using this?
 * @return array
 *  Success message to user.
 */
function github_pages_command_update($arguments, $options) {
  github_pages_path_alias_update(TRUE);
  github_pages_path_alias_clean(TRUE);
  b_set_message(bt("Updated!"), 'success');
}
