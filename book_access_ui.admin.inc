<?php
// $Id$

/**
 * @file
 *
 * Administration interface for Book access module.
 */

/**
 * Form builder for the book access configuration page.
 *
 * @see book_access_ui_user_add_submit()
 * @see book_access_ui_grants_form_submit()
 * @see theme_book_access_ui_grants_form()
 * @ingroup forms
 */
function book_access_ui_grants_form($form_state, $node) {
  $grant_ids = array(
    'grant_view', 'grant_update', 'grant_delete',
    'grant_admin_access', 'grant_add_child', 'grant_edit_outline',
  );

  $bid = $node->book['bid'];
  $context = array('bid' => $bid, 'node' => clone $node);
  $form = array(
    '#bid' => $bid,
    '#author' => $node->uid,
    '#rids' => array(),
    '#uids' => array(),
    '#tree' => TRUE,
  );
  $vars = new BookAccessVars();

  // Build the role access permissions for the book.

  $grants = db_fetch_array(db_query_range(
    'SELECT * FROM {book_access_author} WHERE nid = %d AND uid = %d',
    $bid, $node->uid, 0, 1)
  );

  if (!grants) {
    $default_author_access = array_filter($vars['book_access_default_author_access']);

    foreach ($grant_ids as $id) {
      $grants[$id] = !empty($default_author_access[$id]);
    }
  }

  drupal_alter('book_access_author_grants', $grants, $context);

  $form['author']['name'] = array(
   '#value' => t('author'),
  );

  $form['author']['grant_view'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_view']),
  );

  $form['author']['grant_update'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_update']),
  );

  $form['author']['grant_delete'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_delete']),
  );

  $form['author']['grant_admin_access'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_admin_access']),
  );

  $form['author']['grant_add_child'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_add_child']),
  );

  $form['author']['grant_edit_outline'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_edit_outline']),
  );

  $grants = array(
    'grant_view' => array(),
    'grant_update' => array(),
    'grant_delete' => array(),
    'grant_admin_access' => array(),
    'grant_add_child' => array(),
    'grant_edit_outline' => array(),
  );
  $roles = user_roles();

  $rids = array_keys($roles);
  $result = db_query(
    "SELECT * FROM {book_access_role} WHERE nid = %d AND rid IN (" . db_placeholders($rids) . ")",
    array_merge(array($bid), $rids)
  );

  // Build the role access permissions for the book.
  while ($book_access = db_fetch_object($result)) {
    unset($roles[$book_access->rid]);

    foreach ($grant_ids as $id) {
      $grants[$id][$book_access->rid] = !empty($book_access->$id);
    }
  }

  $default_roles_access = array_filter($vars['book_access_default_roles_access']);

  // Set the default role access permissions for the roles that don't have
  // access permissions already set.
  foreach ($grant_ids as $id) {
    foreach ($roles as $rid => $name) {
      $grants[$id][$rid] = !empty($default_roles_access[$id]);
    }
  }

  $roles = user_roles();
  $data = &$roles;
  $data['__drupal_alter_by_ref'] = array(&$grants);
  drupal_alter('book_access_roles_grants', $data, $context);

  foreach ($roles as $rid => $name) {
    $form['#rids'][] = $rid;

    $form['roles']['names'][$rid] = array(
      '#value' => check_plain($name),
    );

    $form['roles']['grant_view'][$rid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_view'][$rid]),
    );

    $form['roles']['grant_update'][$rid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_update'][$rid]),
    );

    $form['roles']['grant_delete'][$rid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_delete'][$rid]),
    );

    $form['roles']['grant_admin_access'][$rid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_admin_access'][$rid]),
    );

    $form['roles']['grant_add_child'][$rid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_add_child'][$rid]),
    );

    $form['roles']['grant_edit_outline'][$rid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_edit_outline'][$rid]),
    );
  }

  $grants = array(
    'grant_view' => array(),
    'grant_update' => array(),
    'grant_delete' => array(),
    'grant_admin_access' => array(),
    'grant_add_child' => array(),
    'grant_edit_outline' => array(),
  );
  $uids = array();

  $result = db_query(
    "SELECT * FROM {book_access_user} bau INNER JOIN {users} u ON u.uid = bau.uid WHERE bau.nid = %d",
    $bid
  );
  while ($book_access = db_fetch_object($result)) {
    $uid = $book_access->uid;
    $uids[$uid] = $uid;

    foreach ($grant_ids as $id) {
      $grants[$id][$uid] = !empty($book_access->$id);
    }
  }

  $data = &$uids;
  $data['__drupal_alter_by_ref'] = array(&$grants);
  drupal_alter('book_access_users_grants', $data, $context);

  foreach ($uids as $uid) {
    $form['#uids'][] = $uid;

    $form['users']['grant_view'][$uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_view'][$uid]),
    );

    $form['users']['grant_update'][$uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_update'][$uid]),
    );

    $form['users']['grant_delete'][$uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_delete'][$uid]),
    );

    $form['users']['grant_admin_access'][$uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_admin_access'][$uid]),
    );

    $form['users']['grant_add_child'][$uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_add_child'][$uid]),
    );

    $form['users']['grant_edit_outline'][$uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($grants['grant_edit_outline'][$uid]),
    );
  }

  $form['user']['username'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#autocomplete_path' => 'user/autocomplete',
  );

  $grants = array_filter($vars['book_access_default_users_access']);

  $form['user']['grant_view'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_view']),
  );

  $form['user']['grant_update'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_update']),
  );

  $form['user']['grant_delete'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_delete']),
  );

  $form['user']['grant_admin_access'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_admin_access']),
  );

  $form['user']['grant_add_child'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_add_child']),
  );

  $form['user']['grant_edit_outline'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($grants['grant_edit_outline']),
  );

  $form['user']['add_user'] = array(
    '#type'  => 'submit',
    '#value' => t('Add user'),
    '#submit' => array('book_access_ui_user_add_submit'),
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Form submission callback for book_access_ui_grants_form().
 *
 * @see book_access_ui_grants_form()
 */
function book_access_ui_user_add_submit($form, &$form_state) {
  if ($user = user_load(array('name' => $form_state['values']['user']['username']))) {
    $uid = $user->uid;
    $grants = array();
    $grants['grant_view'][$uid] = $form_state['values']['user']['grant_view'];
    $grants['grant_update'][$uid] = $form_state['values']['user']['grant_update'];
    $grants['grant_delete'][$uid] = $form_state['values']['user']['grant_delete'];
    $grants['grant_admin_access'][$uid] = $form_state['values']['user']['grant_admin_access'];
    $grants['grant_add_child'][$uid] = $form_state['values']['user']['grant_add_child'];
    $grants['grant_edit_outline'][$uid] = $form_state['values']['user']['grant_edit_outline'];

    book_access_user_add($form['#bid'], array($uid), $grants);
    _book_access_ui_permissions_warning();
  }
  else {
    drupal_set_message(t("User %user has not been found.", array('%user' => $username)), 'error');
  }
}

/**
 * Form submission callback for book_access_ui_grants_form().
 *
 * @see book_access_ui_grants_form()
 */
function book_access_ui_grants_form_submit($form, &$form_state) {
  $bid = $form['#bid'];

  db_query("DELETE FROM {book_access_author} WHERE nid = %d", $bid);
  db_query("DELETE FROM {book_access_role} WHERE nid = %d", $bid);
  db_query("DELETE FROM {book_access_user} WHERE nid = %d", $bid);

  book_access_author_add($bid, $form['#author'], $form_state['values']['author']);
  book_access_role_add($bid, $form['#rids'], $form_state['values']['roles']);

  if (!empty($form['#uids'])) {
    book_access_user_add($bid, $form['#uids'], $form_state['values']['users']);
  }

  $batch = array(
    'title' => t('Rebuilding book access permissions'),
    'operations' => array(
      array('_book_access_rebuild_confirm_batch_operation', array($bid)),
    ),
    'finished' => '_book_access_rebuild_confirm_batch_finished',
    'file' => drupal_get_path('module', 'book_access') . '/book_access_admin.inc',
  );

  batch_set($batch);
}

/**
 * Form builder for the remove permissions confirmation form.
 *
 * @see book_access_ui_user_permissions_remove_submit()
 * @ingroup forms
 */
function book_access_ui_user_permissions_remove($form_state, $node, $user) {
  $form = array(
    '#bid' => $node->book['bid'],
    '#uid' => $user->uid,
  );

  if ($top_node = node_load($node->book['bid'])) {
    $title = $top_node->title;
  }
  else {
    $title = '#' . $node->book['bid'];
  }

  return confirm_form(
    $form,
    t('Are you sure you want to remove the user %user permissions for the book %title?', array('%user' => $user->name, '%title' => $title)),
    'node/' . $node->book['bid'] . '/book_access',
     t('This action cannot be undone.'), t('Remove')
  );
}

/**
 * Form submission handler for book_access_ui_user_permissions_remove().
 *
 * @see book_access_ui_user_permissions_remove()
 */
function book_access_ui_user_permissions_remove_submit($form, &$form_state) {
  db_query(
    "DELETE FROM {book_access_user} WHERE nid = %d AND uid = %d",
    $form['#bid'], $form['#uid']
  );

  _book_access_ui_permissions_warning();

  $form_state['redirect'] = 'node/' . $form['#bid'] . '/book_access';
}

/**
 * Themes the permission tab added to each book.
 *
 * @param $form
 *   The form to render.
 *
 * @see book_access_ui_grants_form()
 * @ingroup themeable
 */
function theme_book_access_ui_grants_form($form) {
  $rows = array();

  if ($form['#author']) {
    $rows[] = array(
      drupal_render($form['author']['name']),
      drupal_render($form['author']['grant_admin_access']),
      drupal_render($form['author']['grant_update']),
      drupal_render($form['author']['grant_delete']),
      drupal_render($form['author']['grant_add_child']),
      drupal_render($form['author']['grant_edit_outline']),
      drupal_render($form['author']['grant_view']),
      ' ',
    );
  }

  if (isset($form['#rids'])) {
    foreach ($form['#rids'] as $key) {
      $row = array();

      $row[] = drupal_render($form['roles']['names'][$key]);
      $row[] = drupal_render($form['roles']['grant_admin_access'][$key]);
      $row[] = drupal_render($form['roles']['grant_update'][$key]);
      $row[] = drupal_render($form['roles']['grant_delete'][$key]);
      $row[] = drupal_render($form['roles']['grant_add_child'][$key]);
      $row[] = drupal_render($form['roles']['grant_edit_outline'][$key]);
      $row[] = drupal_render($form['roles']['grant_view'][$key]);
      $row[] = ' ';

      $rows[] = $row;
    }
  }

  if (isset($form['#uids'])) {
    foreach ($form['#uids'] as $key) {
      $row = array();
      $user = user_load($key);

      $row[] = theme('username', $user);
      $row[] = drupal_render($form['users']['grant_admin_access'][$key]);
      $row[] = drupal_render($form['users']['grant_update'][$key]);
      $row[] = drupal_render($form['users']['grant_delete'][$key]);
      $row[] = drupal_render($form['users']['grant_add_child'][$key]);
      $row[] = drupal_render($form['users']['grant_edit_outline'][$key]);
      $row[] = drupal_render($form['users']['grant_view'][$key]);
      $row[] = l(t('delete'), 'book_access/delete/user_permission/' . $form['#bid'] . "/$key");

      $rows[] = $row;
    }
  }

  $rows[] = array(
    drupal_render($form['user']['username']),
    drupal_render($form['user']['grant_admin_access']),
    drupal_render($form['user']['grant_update']),
    drupal_render($form['user']['grant_delete']),
    drupal_render($form['user']['grant_add_child']),
    drupal_render($form['user']['grant_edit_outline']),
    drupal_render($form['user']['grant_view']),
    drupal_render($form['user']['add_user']),
  );

  $header = array(
    t('Role/user'),
    t('Administer access'),
    t('Edit pages'),
    t('Delete pages'),
    t('Add child pages'),
    t('Edit outline'),
    t('View this book'),
    t('Operations'),
  );

  return theme('table', $header, $rows) . drupal_render($form);
}

/**
 * Themes the settings fieldset added to the book settings page.
 *
 * @param $form
 *   The form to render.
 *
 * @see book_access_ui_form_book_admin_settings_alter()
 * @ingroup themeable
 */
function theme_book_access_ui_settings($form) {
  $header = array(t('Access type'));
  $options = $form['#options'];
  $rows = array();

  foreach ($options as $option => $title) {
    $header[] = $title;
  }

  foreach (element_children($form['grants']) as $key) {
    $row = array();

    $row[] = $form['grants'][$key]['#title'];
    unset($form['grants'][$key]['#title']);

    foreach ($options as $option => $title) {
      $row[] = drupal_render($form['grants'][$key][$option]);
    }

    $rows[] = $row;
  }

  return drupal_render($form['info']) . theme('table', $header, $rows) . drupal_render($form);
}

/**
 * Shows the warning message that reminds the users with the permissions to
 * edit the book permissions that they need to save the settings.
 */
function _book_access_ui_permissions_warning() {
  drupal_set_message(t("The changes will not be saved until the <em>Save configuration</em> button is clicked."), 'warning');
}

function _book_access_ui_rebuild_batch_operation($bid, &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_node'] = 0;
    $context['sandbox']['max'] = db_result(db_query('SELECT COUNT(DISTINCT nid) FROM {book} WHERE bid = %d', $bid));
  }

  $result = db_query_range(
    "SELECT nid FROM {book} WHERE nid > %d AND bid = %d ORDER BY nid ASC",
    $context['sandbox']['current_node'], $bid, 0, 20
  );

  while ($row = db_fetch_array($result)) {
    $loaded_node = node_load($row['nid'], NULL, TRUE);

    if (!empty($loaded_node)) {
      node_access_acquire_grants($loaded_node);
    }
    $context['sandbox']['progress']++;
    $context['sandbox']['current_node'] = $row['nid'];
    $context['message'] = t('Rebuilding access permissions for %title', array('%title' => $loaded_node->title));
  }

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

function _book_access_ui_rebuild_batch_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('The book access permissions have been rebuilt.'));
    node_access_needs_rebuild(FALSE);
  }
  else {
    drupal_set_message(t('The book access permissions have not been properly rebuilt.'), 'error');
  }
  cache_clear_all();
}
